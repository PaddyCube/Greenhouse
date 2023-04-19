#include "smartGreenhouse.h"
#include <PubSubClient.h>
// #include <Adafruit_BME280.h>
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

    mqttLastReconnect = 999999999;
    mqtt_client.setClient(espClient);
    mqtt_client.setServer(settings.mqtt_server, settings.mqtt_port);
    mqtt_client.setBufferSize(1024);
    mqtt_client.setCallback(std::bind(&SmartGreenhouse::mqtt_callback, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    // set IO pins
    pinMode(pin_window1_closed, INPUT_PULLUP);
    pinMode(pin_window1_open, INPUT_PULLUP);
    pinMode(pin_window2_closed, INPUT_PULLUP);
    pinMode(pin_window2_open, INPUT_PULLUP);
    pinMode(pin_door_open, INPUT_PULLUP);
    pinMode(pin_door_closed, INPUT_PULLUP);

    // setup motor
    motors = new L298N(pin_motor_enable, pin_motor1_in1, pin_motor1_in2, pin_motor2_in1, pin_motor2_in2);
    motors->setSpeed(settings.window_speed);
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

    for (int i = 0; i < NUM_OF_WINDOWS; i++)
    {
        window_open[i] = false;
        window_closed[i] = false;
        window_position[i] = -1;
        window_last_target_position[i] = -1;
        window_target_position[i] = -1;
        window_last_time_new_target[i] = 999999999;

        time_motor_current_zero[i] = 99999999;
        // get motor zero value
        // motor_current_zeroVoltage[i] = getMotorCurrentZeroVoltage(i);
        time_last_motor_current[i] = 999999999;
        motor_last_voltage[i] = 0.0;
    }
    endstops_open[0] = new button_debounce(pin_window1_open, WINDOW_ENDSTOPS_NC);
    endstops_open[1] = new button_debounce(pin_window2_open, WINDOW_ENDSTOPS_NC);
    endstops_closed[0] = new button_debounce(pin_window1_closed, WINDOW_ENDSTOPS_NC);
    endstops_closed[1] = new button_debounce(pin_window2_closed, WINDOW_ENDSTOPS_NC);
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
    if (settings.enableWindows)
    {
        if (operation_state == AUTO && (NUM_OF_WINDOWS > 0 && error_state != WINDOW_0_ENDSTOP_ERROR && error_state != WINDOW_1_ENDSTOP_ERROR && error_state != WINDOW_MOVE_ERROR))
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
            if (temperature > settings.window_min_temp && temperature < settings.window_max_temp)
            {
                float steps = (settings.window_max_temp - settings.window_min_temp) / (settings.max_window_positions);
                float offset = temperature - settings.window_min_temp;
                target_position = offset / steps;
            }

            for (int i = 0; i < NUM_OF_WINDOWS; i++)
            {
                moveWindow(i, target_position);
            }
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
    if (operation_state == MANUAL)
    {
        if (now - mqtt_last_time >= MQTT_MANUAL_SEND_INTERVAL)
            mqtt_trigger = true;
    }

    if (now - mqtt_last_time >= settings.mqtt_send_interval * 1000 || mqtt_trigger == true)
    {
        mqtt_trigger = false;
        mqtt_last_time = millis();
        sendMqttData();
        writeLCD();
    }

    mqtt_client.loop();
    toggleOTHERS();
    endstops_open[0]->loop();
    endstops_open[1]->loop();
    endstops_closed[0]->loop();
    endstops_closed[1]->loop();
}
/*--------------------------------------------------------------*/
void SmartGreenhouse::mqtt_reconnect()
{
    // no reconnect when motor is turning
    if (motors->motor_enable[0] || motors->motor_enable[1])
    {
        // return;
    }

    // don't try to reconnect continuously
    if (millis() - mqttLastReconnect > 60 * 1000)
    {
        mqttLastReconnect = millis();
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
    unsigned int now = millis();

    if (window >= 0 && window < NUM_OF_WINDOWS)
    {
        // only set a new target, if we know the actual position
        if (window_position[window] != -1)
        {
            if (position >= 0 && position <= settings.max_window_positions &&
                now - window_last_time_new_target[window] > settings.window_new_goal_timeout * 1000) // only accept new targets if more than 5 sec done since last time
            {
                window_target_position[window] = position;
                window_last_time_new_target[window] = now;

                // check if we got a new target
                if (window_last_target_position[window] != window_target_position[window])
                {
                    Serial.print("Start moving windows to ");
                    Serial.println(window_target_position[window]);

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
}

/*--------------------------------------------------------------*/
void SmartGreenhouse::getSensorData()
{
    if (NUM_OF_DOORS != 0)
        getDoorState();

    if (NUM_OF_WINDOWS > 0)
        getWindowState();

    checkButton();

    if (millis() - time_last_sensor_read > TIMEOUT_READ_SENSOR)
    {

        time_last_sensor_read = millis();
        getVoltage();

        // get BME data
        temperature = bme.readTemperature();
        humidity = bme.readHumidity();
        if (temperature == 0.0 && humidity == 0.0)
        {
            error_state = BME_ERROR;
        }
    }
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
    for (int i = 0; i < NUM_OF_WINDOWS; i++)
    {
        // if (error_state == WINDOW_0_ENDSTOP_ERROR || error_state == WINDOW_1_ENDSTOP_ERROR)
        // {
        //     error_state = OK;
        // }
        window_open[i] = endstops_open[i]->get_state();
        window_closed[i] = endstops_closed[i]->get_state();

        if (window_open[i] == true && window_closed[i] == true)
        {
            switch (i)
            {
            case 0:
                error_state = WINDOW_0_ENDSTOP_ERROR;
                mqtt_trigger = true;
                break;
            case 1:
                error_state = WINDOW_1_ENDSTOP_ERROR;
                mqtt_trigger = true;
                break;
            }
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
        solarVoltage = raw * (3.3 / 4095);
        solarVoltage = solarVoltage * calib_factor;
    }

    if (BATTERY_MONITOR)
    {
        // Voltage divider R1 10k and R2=1k
        float calib_factor = 12.32; // (R1 + R1) / R2
        unsigned long raw = analogRead(pin_vcc_battery);
        batVoltage = raw * (3.3 / 4095);
        batVoltage = batVoltage * calib_factor;
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
            if (error_state == NO_ERROR)
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
            else
            {
                // ensure not to heat when we have an error
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
    unsigned int now = millis();
    for (int i = 0; i < NUM_OF_WINDOWS; i++)
    {
        // check for motor zero voltage, but only if they're not turning
        // if (now - time_motor_current_zero[i] > MOTOR_CURRENT_ZERO_TIMEOUT && !motors->motor_enable[i])
        // {
        //     motor_current_zeroVoltage[i] = getMotorCurrentZeroVoltage(i);
        //     time_motor_current_zero[i] = now;
        // }

        // in case of error, stop motors
        if (error_state != NO_ERROR)
        {
            motors->stop(i);
        }
        else
        {
            // home routine, if position is unknown
            if (window_position[i] == -1)
            {
                motors->runMotor(i, backward);
            }

            // check motor endstop closed position
            // when we close window, stop at endstop in any case
            if (window_target_position[i] < window_position[i] || window_position[i] == -1)
            {
                if (window_closed[i])
                {
                    motors->stop(i);
                    Serial.print("Window ");
                    Serial.print(i);
                    Serial.print("endstop CLOSE reached, position: ");
                    Serial.print(window_position[i]);
                    Serial.print(", target pos: ");
                    Serial.println(window_target_position[i]);
                    window_position[i] = 0;
                    window_target_position[i] = 0;
                    window_last_target_position[i] = 0;
                }
            }
            else
            {
                // when we open window, stop at endstop in any case
                if (window_target_position[i] > 0 && motors->motor_enable[i])
                {
                    if (window_open[i])
                    {
                        motors->stop(i);
                        window_position[i] = settings.max_window_positions;
                        window_target_position[i] = settings.max_window_positions;
                        window_last_target_position[i] = settings.max_window_positions;
                        Serial.print("Window ");
                        Serial.print(i);
                        Serial.println("endstop MAX OPEN reached");
                        return;
                    }
                }
            }

            // when target equals actual position, stop motors
            if (window_target_position[i] != window_position[i])
            {
                if (!motors->motor_enable[i])
                {
                    window_position[i] = window_target_position[i];
                    Serial.print("Window ");
                    Serial.print(i);
                    Serial.println("target pos reached");
                }
            }

            // when target position equals 0 but no endpoint has been reached, continue running
            if (window_target_position[i] == 0 && window_closed[i] == false && !motors->motor_enable[i])
            {
                motors->runMotor(i, backward);
            }

            // when target position equals 0 and endstop has been reached, disable motor
            if (window_target_position[i] == 0 && window_closed[i] == true && motors->motor_enable[i])
            {
                motors->stop(i);
            }

            // when target position equals max but no endpoint has been reached, continue running
            if (window_target_position[i] == settings.max_window_positions && window_open[i] == false && !motors->motor_enable[i])
            {
                motors->runMotor(i, forward);
            }

            // when target position equals 0 and endstop has been reached, disable motor
            if (window_target_position[i] == settings.max_window_positions && window_open[i] == true && motors->motor_enable[i])
            {
                motors->stop(i);
            }
        }

        // check overload condition
        // if (motors->motor_enable[i])
        // {
        //     int motorCurrent = 0;
        //     if (now - time_last_motor_current[i] > time_interval_motor_current)
        //     {
        //         motorCurrent = getMotorCurrent(i);
        //         time_last_motor_current[i] = now;
        //         if (motorCurrent > MOTOR_MAX_CURRENT)
        //         {
        //             Serial.print("Overload Motor ");
        //             Serial.print(i);
        //             Serial.print(": ");
        //             Serial.println(motorCurrent);
        //             motors->stop(i);
        //             error_state = WINDOW_MOVE_ERROR;
        //         }
        //     }
        // }
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
    mqtt_trigger = true;
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
/*--------------------------------------------------------------*/
void SmartGreenhouse::initBME()
{
    Serial.println("init BME");
    if (!bme.begin(0x76))
    {
        error_state = BME_ERROR;
    }
    Serial.println("init BME done");
}
/*--------------------------------------------------------------*/
void SmartGreenhouse::initLCD()
{
    lcd = new LiquidCrystal_I2C(0x27, 20, 4);
    lcd->init();
    lcd->backlight();
    lcd->blink_off();
}

/*--------------------------------------------------------------*/
void SmartGreenhouse::writeLCD()
{
    lcdWriteCounter++;

    // clear display to get rid of junk output.
    if (lcdWriteCounter > lcdClearCounter)
    {
        lcd->clear();
        lcdWriteCounter = 0;
    }

    char str[10];

    lcd->setCursor(0, 0);
    sprintf(str, "T %3.2fC", temperature);
    lcd->print(str);

    lcd->setCursor(10, 0);
    sprintf(str, "H %3.0f", humidity);
    lcd->print(str);

    lcd->setCursor(17, 0);
    if (operation_state == MANUAL)
    {
        lcd->print("M");
    }
    else
    {
        lcd->print("A");
    }

    lcd->setCursor(18, 0);
    sprintf(str, "E%i", error_state);
    lcd->print(str);

    // second row
    if (BATTERY_MONITOR)
    {
        lcd->setCursor(0, 1);
        sprintf(str, "B %2.2f V", batVoltage);
        lcd->print(str);
    }

    if (SOLAR_POWERED)
    {
        lcd->setCursor(10, 1);
        sprintf(str, "S %2.2f V", solarVoltage);
        lcd->print(str);
    }

    if (USE_WATER_PUMP)
    {
        lcd->setCursor(0, 2);
        sprintf(str, "Water  %i", water_pump_enable);
        lcd->print(str);
    }

    if (USE_HEATER)
    {
        lcd->setCursor(10, 2);
        sprintf(str, "Heater %i", heater_enable);
        lcd->print(str);
    }
    if (USE_FAN)
    {
        lcd->setCursor(0, 3);
        sprintf(str, "Fan    %i", fan_enable);
        lcd->print(str);
    }

    if (USE_OTHERS)
    {
        lcd->setCursor(10, 3);
        sprintf(str, "Others %i", others_enable);
        lcd->print(str);
    }
}

/*--------------------------------------------------------------*/
bool SmartGreenhouse::relaisStatus(int num)
{

    switch (num)
    {
    case HEATER:
        return heater_enable;
        break;

    case WATER:
        return water_pump_enable;
        break;

    case FAN:
        return fan_enable;
        break;

    case LIGHT:
        return light_enable;
        break;

    case OTHERS:
        return others_enable;
        break;
    }
    return false;
}

/*--------------------------------------------------------------*/
void SmartGreenhouse::toggleOTHERS()
{
    if (USE_OTHERS)
    {
        if ((heater_enable && COMBINE_OTHERS_HEATER) ||
            (fan_enable && COMBINE_OTHERS_FAN) ||
            (water_pump_enable && COMBINE_OTHERS_WATER) ||
            (light_enable && COMBINE_OTHERS_LIGHT) ||
            (motors->motor_enable[0] && COMBINE_OTHERS_MOTORS) ||
            (motors->motor_enable[1] && COMBINE_OTHERS_MOTORS))
        {
            toggleRelais(OTHERS, true);
        }
        else
        {
            toggleRelais(OTHERS, false);
        }
    }
}

/*--------------------------------------------------------------*/
int SmartGreenhouse::getMotorCurrent(int motor)
{
    int raw_value = 0;
    int current = 0;
    float voltage = 0.0;
    float lastVoltage = 0.0;
    float voltageDifference = 0.0;
    float zeroVoltageDivider = 0.0;

    if (motor >= 0 && motor <= NUM_OF_WINDOWS)
    {
        lastVoltage = motor_last_voltage[motor];
        switch (motor)
        {
        case 0:
            raw_value = analogRead(pin_motor1_current);
            voltage = raw_value * (3.3 / 4095);
            // read the last voltage we measured and take it into account
            if (lastVoltage > 0.0)
            {
                voltage = (voltage + lastVoltage) / 2;
            }
            motor_last_voltage[motor] = voltage;
            // voltage before voltage divider
            voltage = voltage * (MOTOR1_R1 + MOTOR1_R2) / MOTOR1_R2;
            zeroVoltageDivider = motor_current_zeroVoltage[motor] * (MOTOR1_R1 + MOTOR1_R2) / MOTOR1_R2;
            break;

        case 1:
            raw_value = analogRead(pin_motor2_current);
            voltage = raw_value * (3.3 / 4095);
            // read the last voltage we measured and take it into account
            if (lastVoltage > 0.0)
            {
                voltage = (voltage + lastVoltage) / 2;
            }
            motor_last_voltage[motor] = voltage;

            // voltage before voltage divider
            voltage = voltage * (MOTOR2_R1 + MOTOR2_R2) / MOTOR2_R2;
            zeroVoltageDivider = motor_current_zeroVoltage[motor] * (MOTOR2_R1 + MOTOR2_R2) / MOTOR2_R2;
            break;
        }
        voltageDifference = abs(voltage - zeroVoltageDivider);
        current = voltageDifference / 0.185 * 1000; // 185mV/A
    }
    return current;
}

/*--------------------------------------------------------------*/
float SmartGreenhouse::getMotorCurrentZeroVoltage(int motor)
{
    int raw_value = 0;

    float voltage = 0.0;
    if (motor >= 0 && motor <= NUM_OF_WINDOWS)
    {
        switch (motor)
        {
        case 0:
            raw_value = analogRead(pin_motor1_current);
            voltage = raw_value * (3.3 / 4095);
            break;

        case 1:
            raw_value = analogRead(pin_motor2_current);
            voltage = raw_value * (3.3 / 4095);
            break;
        }
        if (motor_current_zeroVoltage[motor] > 0.0)
        {
            voltage = (voltage + motor_current_zeroVoltage[motor] / 2);
        }
    }
    return voltage;
}

/*--------------------------------------------------------------*/
void SmartGreenhouse::loadSettings()
{
    motors->setSpeed(settings.window_speed);
}

