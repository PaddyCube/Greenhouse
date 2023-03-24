#include <FS.h> //this needs to be first, or it all crashes and burns...
#include <Arduino.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include "config.h"
#include "defaults.h"
#include "wmIntParameter.h"
#include "smartGreenhouse.h"
#include "Button2.h"

#ifdef ESP32
#include <SPIFFS.h>
#endif

#include <ArduinoJson.h> //https://github.com/bblanchon/ArduinoJson

// wifimanager can run in a blocking mode or a non blocking mode
// Be sure to know how to process loops with no delay() if using non blocking
bool wm_nonblocking = true; // change to true to use non blocking

WiFiManager wm;             // global wm instance
SmartGreenhouse greenhouse; // global Greenhouse instance

WiFiManagerParameter custom_mqtt_server("mqttserverID", "MQTT Server", greenhouse.settings.mqtt_server, 40);
IntParameter custom_mqtt_port("mqttportID", "MQTT Port", greenhouse.settings.mqtt_port);
WiFiManagerParameter custom_mqtt_api("mqttapiID", "MQTT API Token", greenhouse.settings.mqtt_api_token, 34);
IntParameter custom_mqtt_interval("mqttinterval", "MQTT send interval", greenhouse.settings.mqtt_send_interval);
IntParameter custom_window_pos("windowPosID", "Num of window positions", greenhouse.settings.max_window_positions);
IntParameter custom_window_min("windowMinTempID", "close window if temp below °C", greenhouse.settings.window_min_temp);
IntParameter custom_window_max("windowMaxTempID", "fully open window if temp above °C", greenhouse.settings.window_max_temp);
IntParameter custom_window_steptime("windowStepTimeID", "time(s)  position", greenhouse.settings.window_step_time);
IntParameter custom_heater_min("heaterMinID", "turn heater on, if temp below °C", greenhouse.settings.heater_min_temp);
IntParameter custom_heater_max("heaterMaxID", "turn heater off, if temp below °C", greenhouse.settings.heater_max_temp);
IntParameter custom_fan("fanID", "turn on fan, if humidity above %", greenhouse.settings.fan_min_humidity);
IntParameter custom_water("waterID", "run water pump for (S)", greenhouse.settings.water_pump_timeout);
IntParameter custom_light("lightiD", "Timeout for light (S)", greenhouse.settings.max_light_on);

// Button
Button2 button;

/*--------------------------------------------------------------*/
void buttonClickHandler(Button2 &b)
{
  greenhouse.toggleRelais(LIGHT, !greenhouse.relaisStatus(LIGHT));
}

/*--------------------------------------------------------------*/
void buttonDoubleClickHandler(Button2 &b)
{
  greenhouse.toggleRelais(WATER, !greenhouse.relaisStatus(WATER));
}

/*--------------------------------------------------------------*/
void buttonTripleClickHandler(Button2 &b)
{
  Serial.println("click detected");
}

/*--------------------------------------------------------------*/
void buttonLongPressHandler(Button2 &b)
{
  wm.startConfigPortal();
  // Serial.println("long click detected");
}

// callback notifying us of the need to save config
void saveParamCallback()
{
  Serial.println("Should save config");
  //  shouldSaveConfig = true;
  // read updated parameters
  strcpy(greenhouse.settings.mqtt_server, custom_mqtt_server.getValue());
  greenhouse.settings.mqtt_port = custom_mqtt_port.getValue();
  strcpy(greenhouse.settings.mqtt_api_token, custom_mqtt_api.getValue());
  greenhouse.settings.mqtt_send_interval = custom_mqtt_interval.getValue();

  greenhouse.settings.max_window_positions = custom_window_pos.getValue();
  greenhouse.settings.window_min_temp = custom_window_min.getValue();
  greenhouse.settings.window_max_temp = custom_window_max.getValue();
  greenhouse.settings.window_step_time = custom_window_steptime.getValue();
  greenhouse.settings.max_light_on = custom_light.getValue();
  greenhouse.settings.heater_min_temp = custom_heater_min.getValue();
  greenhouse.settings.heater_max_temp = custom_heater_max.getValue();
  greenhouse.settings.fan_min_humidity = custom_fan.getValue();
  greenhouse.settings.water_pump_timeout = custom_water.getValue();

  // build JSON
#if defined(ARDUINOJSON_VERSION_MAJOR) && ARDUINOJSON_VERSION_MAJOR >= 6
  DynamicJsonDocument json(1024);
#else
  DynamicJsonBuffer jsonBuffer;
  JsonObject &json = jsonBuffer.createObject();
#endif
  json["mqtt_server"] = greenhouse.settings.mqtt_server;
  json["mqtt_port"] = greenhouse.settings.mqtt_port;
  json["mqtt_api_token"] = greenhouse.settings.mqtt_api_token;
  json["mqtt_interval"] = greenhouse.settings.mqtt_send_interval;

  json["max_window_positions"] = greenhouse.settings.max_window_positions;
  json["window_min_temp"] = greenhouse.settings.window_min_temp;
  json["window_max_temp"] = greenhouse.settings.window_max_temp;
  json["window_step_time"] = greenhouse.settings.window_step_time;
  json["max_light_on"] = greenhouse.settings.max_light_on;
  json["heater_min_temp"] = greenhouse.settings.heater_min_temp;
  json["heater_max_temp"] = greenhouse.settings.heater_max_temp;
  json["fan_min_humidity"] = greenhouse.settings.fan_min_humidity;
  json["water_pump_timeout"] = greenhouse.settings.water_pump_timeout;

  File configFile = SPIFFS.open("/config.json", "w");
  if (!configFile)
  {
    Serial.println("failed to open config file for writing");
  }

#if defined(ARDUINOJSON_VERSION_MAJOR) && ARDUINOJSON_VERSION_MAJOR >= 6
  serializeJson(json, Serial);
  serializeJson(json, configFile);
#else
  json.printTo(Serial);
  json.printTo(configFile);
#endif
  configFile.close();
}

void readConfigFile()
{
  if (SPIFFS.begin())
  {
    Serial.println("mounted file system");
    if (SPIFFS.exists("/config.json"))
    {
      // file exists, reading and loading
      Serial.println("reading config file");
      File configFile = SPIFFS.open("/config.json", "r");
      if (configFile)
      {
        Serial.println("opened config file");
        size_t size = configFile.size();
        // Allocate a buffer to store contents of the file.
        std::unique_ptr<char[]> buf(new char[size]);

        configFile.readBytes(buf.get(), size);

#if defined(ARDUINOJSON_VERSION_MAJOR) && ARDUINOJSON_VERSION_MAJOR >= 6
        DynamicJsonDocument json(1024);
        auto deserializeError = deserializeJson(json, buf.get());
        serializeJson(json, Serial);
        if (!deserializeError)
        {
#else
        DynamicJsonBuffer jsonBuffer;
        JsonObject &json = jsonBuffer.parseObject(buf.get());
        json.printTo(Serial);
        if (json.success())
        {
#endif
          Serial.println("\nparsed json");
          strcpy(greenhouse.settings.mqtt_server, json["mqtt_server"]);
          greenhouse.settings.mqtt_port = json["mqtt_port"];
          strcpy(greenhouse.settings.mqtt_api_token, json["mqtt_api_token"]);
          greenhouse.settings.mqtt_send_interval = json["mqtt_interval"];

          greenhouse.settings.max_window_positions = json["max_window_positions"];
          greenhouse.settings.window_min_temp = json["window_min_temp"];
          greenhouse.settings.window_max_temp = json["window_max_temp"];
          greenhouse.settings.window_step_time = json["window_step_time"];
          greenhouse.settings.max_light_on = json["max_light_on"];
          greenhouse.settings.heater_min_temp = json["heater_min_temp"];
          greenhouse.settings.heater_max_temp = json["heater_max_temp"];
          greenhouse.settings.fan_min_humidity = json["fan_min_humidity"];
          greenhouse.settings.water_pump_timeout = json["water_pump_timeout"];
        }
        else
        {
          Serial.println("failed to load json config");
        }
        configFile.close();
      }
    }
  }
  else
  {
    Serial.println("failed to mount FS. Try to format...");
    SPIFFS.format();
    Serial.println("Formatting complete, please restart");
  }
}

void buildMenuParameters()
{
  custom_mqtt_server.setValue(greenhouse.settings.mqtt_server, 40);
  custom_mqtt_port.setValue(greenhouse.settings.mqtt_port);
  custom_mqtt_api.setValue(greenhouse.settings.mqtt_api_token, 34);
  custom_mqtt_interval.setValue(greenhouse.settings.mqtt_send_interval);
  custom_window_pos.setValue(greenhouse.settings.max_window_positions);
  custom_window_min.setValue(greenhouse.settings.window_min_temp);
  custom_window_max.setValue(greenhouse.settings.window_max_temp);
  custom_window_steptime.setValue(greenhouse.settings.window_step_time);
  custom_heater_min.setValue(greenhouse.settings.heater_min_temp);
  custom_heater_max.setValue(greenhouse.settings.heater_max_temp);
  custom_fan.setValue(greenhouse.settings.fan_min_humidity);
  custom_water.setValue(greenhouse.settings.water_pump_timeout);
  custom_light.setValue(greenhouse.settings.max_light_on);

  wm.addParameter(&custom_mqtt_server);
  wm.addParameter(&custom_mqtt_port);
  wm.addParameter(&custom_mqtt_api);
  wm.addParameter(&custom_mqtt_interval);

  wm.addParameter(&custom_window_pos);
  wm.addParameter(&custom_window_min);
  wm.addParameter(&custom_window_max);
  wm.addParameter(&custom_window_steptime);

#ifdef USE_HEATER
  wm.addParameter(&custom_heater_min);
  wm.addParameter(&custom_heater_max);
#endif

#ifdef USE_FAN
  wm.addParameter(&custom_fan);
#endif

#ifdef USE_WATER_PUMP
  wm.addParameter(&custom_water);
#endif

#ifdef USE_LIGHT
  wm.addParameter(&custom_light);
#endif
}

void setup()
{
  Serial.begin(115200);
  Serial.setDebugOutput(true);
  Serial.println("\n Starting");
  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  delay(3000);

  if (wm_nonblocking)
    wm.setConfigPortalBlocking(false);

  // read configuration file, default values if not exists
  readConfigFile();
  buildMenuParameters();

  wm.setSaveParamsCallback(saveParamCallback);

  // custom menu via array or vector
  //
  // menu tokens, "wifi","wifinoscan","info","param","close","sep","erase","restart","exit" (sep is seperator) (if param is in menu, params will not show up in wifi page!)
  // const char* menu[] = {"wifi","info","param","sep","restart","exit"};
  // wm.setMenu(menu,6);
  std::vector<const char *> menu = {"wifi", "info", "param", "sep", "restart", "exit"};
  wm.setMenu(menu);

  // set dark theme
  wm.setClass("invert");

  wm.setConfigPortalTimeout(300); // auto close configportal after n seconds
  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  wm.setConnectTimeout(30);
  res = wm.autoConnect("smartGreenhouse"); // anonymous ap

  if (!res)
  {
    Serial.println("Failed to connect or hit timeout");
    // ESP.restart();
  }
  else
  {
    // if you get here you have connected to the WiFi
    Serial.println("connected...yeey :)");
  }
  wm.startConfigPortal();
  greenhouse.initBME();
  greenhouse.initLCD();

  // Button
  button.begin(pin_light_switch);
  button.setDebounceTime(100);
  button.setLongClickTime(3000);
  button.setDoubleClickTime(600);

  button.setClickHandler(buttonClickHandler);
  button.setDoubleClickHandler(buttonDoubleClickHandler);
  button.setTripleClickHandler(buttonTripleClickHandler);
  button.setLongClickDetectedHandler(buttonLongPressHandler);
}

void loop()
{
  if (wm_nonblocking)
    wm.process(); // avoid delays() in loop when non-blocking and other long running code

  greenhouse.loop();
  button.loop();
}