#include "smartGreenhouse.h"
#include <PubSubClient.h>
#include <Adafruit_BME280.h>
#include <string.h>
#include <Wire.h>
#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson

#define ARDUINOJSON_ENABLE_STD_STRING 1

/*--------------------------------------------------------------*/
SmartGreenhouse::SmartGreenhouse()
{
    // Initialize timers
    mqtt_last_time =
        time_water_pump =
            time_motor1_on =
                time_motor2_on = millis();

    mqtt_client.setClient(espClient);
    mqtt_client.setServer(settings.mqtt_server, settings.mqtt_port);
    mqtt_client.setBufferSize(1024);
    mqtt_client.setCallback(std::bind(&SmartGreenhouse::mqtt_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    for (int i = 0; i < NUM_OF_WINDOWS; i++)
    {
        window_open[i] = false;
        window_closed[i] = false;
        window_position[i] = -1;
        window_last_target_position[i] = -1;
        window_target_position[i] = -1;
        // time_window_move_start[i] = 0;
    }

    // set IO pins
    pinMode(pin_window1_closed, INPUT_PULLUP);
    pinMode(pin_window1_open, INPUT_PULLUP);
    pinMode(pin_window2_closed, INPUT_PULLUP);
    pinMode(pin_window2_open, INPUT_PULLUP);
    pinMode(pin_door_open, INPUT_PULLUP);
    pinMode(pin_door_closed, INPUT_PULLUP);

    // setup motor
    motors = new L298N(pin_motor_enable, pin_motor1_in1, pin_motor1_in2, pin_motor2_in1, pin_motor2_in2);
    pinMode(pin_motor1_current, INPUT); // motor 1 current sense
    pinMode(pin_motor2_current, INPUT); // motor 2 current sense

    // voltage measurement
    pinMode(pin_vcc_solar, INPUT);
    pinMode(pin_vcc_battery, INPUT);

    // light and switch
    pinMode(pin_light_switch, INPUT_PULLUP); // switch for user input
    pinMode(pin_light_load, OUTPUT);         // relais to control light

    // other loads like fans, water pumps, heaters
    pinMode(pin_relais1, OUTPUT); // heater
    pinMode(pin_relais2, OUTPUT); // fan
    pinMode(pin_relais3, OUTPUT); // water pump
    pinMode(pin_relais4, OUTPUT);

    // turn all off
    toggleRelais(HEATER, false);
    toggleRelais(FAN, false);
    toggleRelais(WATER, false);
    toggleRelais(OTHERS, false);


    // initialize BME

}
/*--------------------------------------------------------------*/
void SmartGreenhouse::loop()
{
    uint32_t now = millis();

    getSensorData();

    // check if manual state is active, turn back to auto
    if (operation_state == MANUAL)
    {
        if (now - time_manual_state > TIMEOUT_MANUAL_STATE)
        {
            setOperationState(AUTO);
        }
    }

    // check temperature and operate windows
    if (operation_state == AUTO && (NUM_OF_WINDOWS > 0 && error_state != WINDOW_ENDSTOP_ERROR && error_state != WINDOW_MOVE_ERROR))
    {
        int target_position = -1;
        if (temperature <= settings.window_min_temp)
        {
            target_position = 0;
        }
        if (temperature > settings.window_max_temp)
        {
            target_position = settings.max_window_positions;
        }
        if (temperature > settings.window_min_temp && temperature < settings.window_min_temp)
        {
            float steps = (settings.window_max_temp - settings.window_min_temp) / settings.max_window_positions;
            float offset = temperature - settings.window_min_temp;
            target_position = static_cast<int>(abs(offset / steps));
        }

        for (int i = 0; i < NUM_OF_WINDOWS; i++)
        {
            moveWindow(i, target_position);
        }
    }

    // check heater
    if (USE_HEATER && operation_state == AUTO)
    {
        if (temperature < settings.heater_min_temp)
        {
            toggleRelais(HEATER, true);
        }
        if (temperature > settings.heater_max_temp)
        {
            toggleRelais(HEATER, false);
        }
    }

    // check fan
    if (USE_FAN && operation_state == AUTO)
    {
        if (humidity > settings.fan_min_humidity)
        {
            toggleRelais(FAN, true);
        }
        else
        {
            toggleRelais(FAN, false);
        }
    }

    // check water
    if (USE_WATER_PUMP && operation_state == AUTO)
    {
        if (now - time_water_pump > settings.water_pump_timeout * 1000 && water_pump_enable)
        {
            toggleRelais(WATER, false);
        }
    }

    // check others
    if (USE_OTHERS && operation_state == AUTO)
    {
    }

    // check light
    if (USE_LIGHT && operation_state == AUTO)
    {
        if (now - time_light > settings.max_light_on * 1000 && light_enable)
        {
            toggleRelais(LIGHT, false);
        }
    }

    // operate motors
    controlMotor();

    // send mqtt data
    if (now - mqtt_last_time >= mqtt_send_interval)
    {
        mqtt_last_time = millis();
        sendMqttData();
    }

    mqtt_client.loop();
}
/*--------------------------------------------------------------*/
void SmartGreenhouse::mqtt_reconnect()
{
    int reconnect_attemps = 0;
    while (!mqtt_client.connected())
    {
        reconnect_attemps++;
        if (reconnect_attemps > 5)
        {
            Serial.println("failed to connect to MQTT Broker");
            return;
        }

        Serial.print("Reconnecting...");
        if (!mqtt_client.connect("smartGreenhouse"))
        {
            Serial.print("failed, rc=");
            Serial.print(mqtt_client.state());
            Serial.println(" retrying in 5 seconds");
            delay(5000);
        }
    }
    Serial.println("connected to MQTT Broker");
    Serial.print("subscribing: ");
    Serial.println(mqtt_client.subscribe(mqtt_subscribe_topic));
}

/*--------------------------------------------------------------*/
void SmartGreenhouse::sendMqttData()
{
    // build JSON
#if defined(ARDUINOJSON_VERSION_MAJOR) && ARDUINOJSON_VERSION_MAJOR >= 6
    DynamicJsonDocument json(1024);
#else
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.createObject();
#endif

    JsonObject environment = json.createNestedObject("environment");
    environment["Temperature"] = temperature;
    environment["Humidity"] = humidity;

    JsonArray windows = json.createNestedArray("windows");
    for (int i = 0; i < NUM_OF_WINDOWS; i++)
    {
        JsonObject window = windows.createNestedObject();
        window["position"] = window_position[i];
        window["target_position"] = window_target_position[i];
        window["moving"] = motors->motor_enable[i];
        window["max_positions"] = settings.max_window_positions;
        window["min_temp"] = settings.window_min_temp;
        window["max_temp"] = settings.window_max_temp;
        window["step_time"] = settings.window_step_time;
        window["closed"] = window_closed[i];
        window["max_opened"] = window_open[i];
    }
    JsonArray doors = json.createNestedArray("doors");
    for (int i = 0; i < NUM_OF_DOORS; i++)
    {
        JsonObject door = doors.createNestedObject();
        door["closed"] = door_closed;
        door["max_opened"] = door_open;
    }
    if (USE_HEATER)
    {
        JsonObject heater = json.createNestedObject("heater");
        heater["min_temp"] = settings.heater_min_temp;
        heater["max_temp"] = settings.heater_max_temp;
        heater["active"] = heater_enable;
    }
    if (USE_FAN)
    {
        JsonObject fan = json.createNestedObject("fan");
        fan["min_humidity"] = settings.fan_min_humidity;
        fan["active"] = fan_enable;
    }
    if (USE_WATER_PUMP)
    {
        JsonObject water = json.createNestedObject("water");
        water["active"] = water_pump_enable;
        water["duration"] = time_water_pump;
    }

    if (USE_OTHERS)
    {
        JsonObject others = json.createNestedObject("others");
        others["active"] = others_enable;
    }
    if (USE_LIGHT)
    {
        JsonObject light = json.createNestedObject("light");
        light["active"] = light_enable;
        light["duration"] = time_light;
    }
    if (SOLAR_POWERED)
    {
        json["solar_voltage"] = solarVoltage;
    }
    if (BATTERY_MONITOR)
    {
        json["battery_voltage"] = batVoltage;
    }

    json["error_code"] = error_names[error_state];
    json["operation_mode"] = operation_state;

    char mqtt_json[1024];
#if defined(ARDUINOJSON_VERSION_MAJOR) && ARDUINOJSON_VERSION_MAJOR >= 6
    serializeJson(json, mqtt_json);
#else
    json.printTo(mqtt_json);
#endif

    if (!mqtt_client.connected())
    {
        mqtt_reconnect();
    }
    else
    {
        mqtt_client.publish(mqtt_topic, mqtt_json);
    }
}

/*--------------------------------------------------------------*/
bool SmartGreenhouse::isDoorOpen()
{
    return door_open;
}

/*--------------------------------------------------------------*/
bool SmartGreenhouse::isWindowOpen(int window)
{
    if (window < NUM_OF_WINDOWS && window >= 0)
    {
        return window_open[window];
    }
    else
    {
        return false;
    }
}

/*--------------------------------------------------------------*/
void SmartGreenhouse::moveWindow(int window, int position)
{
    if (window >= 0 && window < NUM_OF_WINDOWS)
    {
        if (position >= 0 && position <= settings.max_window_positions)
        {
            window_target_position[window] = position;

            // check if we got a new target
            if (window_last_target_position[window] != window_target_position[window])
            {
                Serial.println("Start moving windows");
                // time_window_move_start[window] = millis(); // to check for timeouts
                int duration = (window_target_position[window] - window_position[window]) * settings.window_step_time * 1000;
                if (duration <= 0)
                {
                    motors->runMotorFor(window, backward, abs(duration));
                }
                else
                {
                    motors->runMotorFor(window, forward, abs(duration));
                }

                window_last_target_position[window] = window_target_position[window];
            }
        }
    }
}

/*--------------------------------------------------------------*/
void SmartGreenhouse::getSensorData()
{
    if (NUM_OF_DOORS != 0)
        getDoorState();

    if (NUM_OF_WINDOWS > 0)
        getWindowState();

    getVoltage();
    checkButton();
    // get BME data
    temperature = bme.readTemperature();
    humidity = bme.readHumidity();
}

/*--------------------------------------------------------------*/
void SmartGreenhouse::getDoorState()
{
    if (DOOR_ENDSTOP_NC)
    {
        door_open = digitalRead(pin_door_open);
        door_closed = digitalRead(pin_door_closed);
    }
    else
    {
        door_open = !digitalRead(pin_door_open);
        door_closed = !digitalRead(pin_door_closed);
    }

    // check for errors
    if (door_open == true && door_closed == true)
    {
        error_state = DOOR_ERROR;
    }
}

/*--------------------------------------------------------------*/
void SmartGreenhouse::getWindowState()
{
    if (NUM_OF_WINDOWS > 0)
    {
        if (WINDOW_ENDSTOPS_NC)
        {
            window_open[0] = digitalRead(pin_window1_open);
            window_closed[0] = digitalRead(pin_window1_closed);
        }
        else
        {
            window_open[0] = !digitalRead(pin_window1_open);
            window_closed[0] = !digitalRead(pin_window1_closed);
        }

        if (window_open[0] == true && window_closed[0] == true)
        {
            error_state = WINDOW_ENDSTOP_ERROR;
        }
    }

    if (NUM_OF_WINDOWS > 1)
    {
        window_open[1] = digitalRead(pin_window2_open);
        window_closed[1] = digitalRead(pin_window2_closed);
        if (window_open[1] == true && window_closed[1] == true)
        {
            error_state = WINDOW_ENDSTOP_ERROR;
        }
    }

    if (error_state != WINDOW_MOVE_ERROR)
    {
        // for (int i = 0; i < NUM_OF_WINDOWS; i++)
        // {
        //     if (window_target_position[i] != window_position[i] && millis() - time_window_move_start[i] > WINDOW_MOVE_TIMEOUT)
        //     {
        //         error_state = WINDOW_MOVE_ERROR;
        //     }
        // }
    }
}

/*--------------------------------------------------------------*/
void SmartGreenhouse::getVoltage()
{
    if (SOLAR_POWERED)
    {
        // Voltage divider R1 = 10k and R2=1k
        float calib_factor = 11; // (R1 + R1) / R2
        unsigned long raw = analogRead(pin_vcc_solar);
        solarVoltage = raw * calib_factor / 1024;
    }

    if (BATTERY_MONITOR)
    {
        // Voltage divider R1 10k and R2=1k
        float calib_factor = 11; // (R1 + R1) / R2
        unsigned long raw = analogRead(pin_vcc_battery);
        batVoltage = raw * calib_factor / 1024;
    }
}
/*--------------------------------------------------------------*/
void SmartGreenhouse::toggleRelais(int num, bool on)
{
    switch (num)
    {
    case HEATER:
        if (USE_HEATER)
        {
            heater_enable = on;
            if (on)
            {
                digitalWrite(pin_relais1, LOW);
            }
            else
            {
                digitalWrite(pin_relais1, HIGH);
            }
        }
        break;
    case FAN:
        fan_enable = on;
        if (USE_FAN)
        {
            if (on)
            {
                digitalWrite(pin_relais2, LOW);
            }
            else
            {
                digitalWrite(pin_relais2, HIGH);
            }
        }
        break;
    case WATER:
        if (USE_WATER_PUMP)
        {
            water_pump_enable = on;
            if (on)
            {
                digitalWrite(pin_relais3, LOW);
                time_water_pump = millis();
            }
            else
            {
                digitalWrite(pin_relais3, HIGH);
                time_water_pump = 0;
            }
        }
        break;
    case OTHERS:
        if (USE_OTHERS)
        {
            others_enable = on;
            if (on)
            {
                digitalWrite(pin_relais4, LOW);
            }
            else
            {
                digitalWrite(pin_relais4, HIGH);
            }
        }
        break;
    case LIGHT:
        if (USE_LIGHT)
        {
            light_enable = on;
            if (on)
            {
                digitalWrite(pin_light_load, HIGH);
                time_light = millis();
            }
            else
            {
                digitalWrite(pin_light_load, LOW);
            }
        }
        break;
    }
}

/*--------------------------------------------------------------*/
void SmartGreenhouse::controlMotor()
{
    for (int i = 0; i < NUM_OF_WINDOWS; i++)
    {
        // in case of error, stop motors
        if (error_state == WINDOW_ENDSTOP_ERROR || error_state == WINDOW_MOVE_ERROR)
        {
            motors->stop(i);
        }
        else
        {

            // check motor endstop closed position
            // when we close window, stop at endstop in any case
            if (window_target_position[i] <= window_position[i] || window_position[i] == -1 || window_target_position[i] == 0)
            {
                if (window_closed[i])
                {
                    motors->stop(i);
                    window_position[i] = 0;
                    window_target_position[i] = 0;
                }
            }

            // when we open window, stop at endstop in any case
            if (window_target_position[i] > 0)
            {
                if (window_open[i])
                {
                    motors->stop(i);
                    window_position[i] = settings.max_window_positions;
                    window_target_position[i] = settings.max_window_positions;
                }
            }

            // when target equals actual position, stop motors
            if (window_target_position[i] != window_position[i])
            {
                if (!motors->motor_enable[i])
                {
                    window_position[i] = window_target_position[i];
                    Serial.println("window target pos reached");
                }
            }

            // when target position equals 0 but no endpoint has been reached, continue running
            if (window_target_position[i] == 0 && window_closed[i] == false && !motors->motor_enable[i])
            {
                motors->runMotor(i, backward);
            }
        }
    }
    motors->loop();
}
/*--------------------------------------------------------------*/
void SmartGreenhouse::mqtt_callback(char *topic, byte *payload, unsigned int length)
{
    Serial.print("Message arrived [");
    Serial.print(topic);
    Serial.print("] ");

    Serial.println();
#if defined(ARDUINOJSON_VERSION_MAJOR) && ARDUINOJSON_VERSION_MAJOR >= 6
    DynamicJsonDocument json(512);
    auto deserializeError = deserializeJson(json, payload);
    serializeJson(json, Serial);
    if (!deserializeError)
    {
#else
    DynamicJsonBuffer jsonBuffer;
    JsonObject &json = jsonBuffer.parseObject(payload);
    json.printTo(Serial);
    if (json.success())
    {
#endif
        Serial.println("\nparsed json");
    }

    std::string command = json["command"];
    std::string parameter = json["parameter"];

    if (command == "resetError")
    {
        Serial.println("reset error state");
        error_state = NO_ERROR;
    }

    if (command == "setMode")
    {
        if (parameter == "AUTO")
        {
            setOperationState(AUTO);
        }
        if (parameter == "MANUAL")
        {
            setOperationState(MANUAL);
        }
    }

    if (command == "WINDOW")
    {
        Serial.print("move window to position: ");
        Serial.println(parameter.c_str());

        int new_pos = std::stoi(parameter);

        for (int i = 0; i < NUM_OF_WINDOWS; i++)
        {
            moveWindow(i, new_pos);
        }
    }

    if (command == "RELAIS_ON" || command == "RELAIS_OFF")
    {
        bool turn_on = false;
        if (command == "RELAIS_ON")
        {
            turn_on = true;
        }

        if (parameter == "HEATER")
        {
            toggleRelais(HEATER, turn_on);
        }
        if (parameter == "FAN")
        {
            toggleRelais(FAN, turn_on);
        }
        if (parameter == "WATER")
        {
            toggleRelais(WATER, turn_on);
        }
        if (parameter == "OTHERS")
        {
            toggleRelais(OTHERS, turn_on);
        }
        if (parameter == "LIGHT")
        {
            toggleRelais(LIGHT, turn_on);
        }
    }
}
/*--------------------------------------------------------------*/
void SmartGreenhouse::checkButton()
{
}

/*--------------------------------------------------------------*/
void SmartGreenhouse::setOperationState(operation_states state)
{
    if (state == MANUAL)
    {
        time_manual_state = millis();
        operation_state = state;
        Serial.println("set operation mode to MANUAL");
    }

    if (state == AUTO)
    {
        time_manual_state = 0;
        operation_state = state;
        Serial.println("set operation mode to AUTO");
    }
}

void SmartGreenhouse::initBME()
{
    Serial.println("init BME");
     if (!bme.begin(0x76))
     {
        error_state = BME_ERROR;
     }
    Serial.println("init BME done");

}