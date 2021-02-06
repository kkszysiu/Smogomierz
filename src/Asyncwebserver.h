#ifdef ARDUINO_ARCH_ESP8266
#include <ESP8266httpUpdate.h>
#elif defined ARDUINO_ARCH_ESP32
#include <Update.h>
#endif

#include <ArduinoJson.h> // 6.9.0 or later
#include "spiffs.h"
#include <FS.h>

const char* www_realm = "Custom Auth Realm";
String authFailResponse = "<meta http-equiv='refresh' content='0; url=/' /> Authentication Failed! <p><a href='/'>Redirect</a></p>";

//void handle_root() {
void handle_root(AsyncWebServerRequest *request) {
    String message = FPSTR(WEB_PAGE_HEADER);
  message.replace("{WEB_PAGE_CSS}", FPSTR(WEB_PAGE_HEADER_CSS));
  message.replace("{Language}", (TEXT_LANG));
  message.replace("{CurrentPageTitle}", (TEXT_INDEX_PAGE));
  message.replace("{IndexPageTitle}", (TEXT_INDEX_PAGE));
  message.replace("{ConfigPageTitle}", (TEXT_CONFIG_PAGE));
  message.replace("{UpdatePageTitle}", (TEXT_UPDATE_PAGE));

  message += FPSTR(WEB_ROOT_PAGE_MEASUREMENTS);

  if (!AUTOUPDATE_ON) {
    if (need_update) {
      message.replace("{WEB_UPDATE_INFO_WARNING}", FPSTR(WEB_UPDATE_INFO_WARNING));
      message.replace("{TEXT_FWUPDATEAVALIBLE}", (TEXT_FWUPDATEAVALIBLE));
      message.replace("{MANUALUPDATEBUTTON}", FPSTR(WEB_UPDATE_BUTTON_MANUALUPDATE));
      message.replace("{TEXT_MANUALUPDATEBUTTON}", (TEXT_MANUALUPDATEBUTTON));
      message.replace("{FWUPDATEBUTTON}", FPSTR(WEB_UPDATE_BUTTON_FWUPDATE));
      message.replace("{TEXT_FWUPDATEBUTTON}", (TEXT_FWUPDATEBUTTON));
      message.replace("{AUTOUPDATEONBUTTON}", FPSTR(WEB_UPDATE_BUTTON_AUTOUPDATEON));
      message.replace("{TEXT_AUTOUPDATEONBUTTON}", (TEXT_AUTOUPDATEONBUTTON));
      message.replace("{TEXT_AUTOUPDATEWARNING}", (TEXT_AUTOUPDATEWARNING));
      message.replace("{TEXT_FWUPDATEBUTTON}", (TEXT_FWUPDATEBUTTON));
    }
    message.replace("{WEB_UPDATE_INFO_WARNING}", "");
  } else {
    message.replace("{WEB_UPDATE_INFO_WARNING}", "");
  }

  if (!strcmp(THP_MODEL, "Non")) {
    message.replace("{TEXT_WEATHER}:", "");
    message.replace("{TEXT_TEMPERATURE}: {Temperature} °C", "");
    message.replace("{TEXT_HUMIDITY}: {Humidity} %", "");
    message.replace("{TEXT_PRESSURE}: {Pressure} hPa", "");
    message.replace("{TEXT_DEWPOINT}: {Dewpoint} °C", "");
  } else {
    takeTHPMeasurements();
    message.replace("{TEXT_WEATHER}", (TEXT_WEATHER));
  }
  if (!strcmp(THP_MODEL, "BME280")) {
    if (checkBmeStatus()) {
      message.replace("{TEXT_TEMPERATURE}", (TEXT_TEMPERATURE));
      message.replace("{TEXT_HUMIDITY}", (TEXT_HUMIDITY));
      message.replace("{TEXT_PRESSURE}", (TEXT_PRESSURE));
      message.replace("{TEXT_DEWPOINT}", (TEXT_DEWPOINT));

      message.replace("{Temperature}", String(int(currentTemperature)));
      message.replace("{Pressure}", String(int(currentPressure)));
      message.replace("{Humidity}", String(int(currentHumidity)));
      message.replace("{Dewpoint}", String(int(pow((currentHumidity) / 100, 0.125) * (112 + 0.9 * (currentTemperature)) + 0.1 * (currentTemperature) - 112)));
    } else {
      message.replace("{TEXT_TEMPERATURE}: {Temperature} °C", "");
      message.replace("{TEXT_HUMIDITY}: {Humidity} %", "");
      message.replace("{TEXT_PRESSURE}: {Pressure} hPa", "");
      message.replace("{TEXT_DEWPOINT}: {Dewpoint} °C", "");
    }
  } else if (!strcmp(THP_MODEL, "HTU21")) {
    if (checkHTU21DStatus()) {
      message.replace("{TEXT_TEMPERATURE}", (TEXT_TEMPERATURE));
      message.replace("{TEXT_HUMIDITY}", (TEXT_HUMIDITY));
      message.replace("{TEXT_DEWPOINT}", (TEXT_DEWPOINT));

      message.replace("{Temperature}", String(int(currentTemperature)));
      message.replace("{TEXT_PRESSURE}: {Pressure} hPa", "");
      message.replace("{Humidity}", String(int(currentHumidity)));
      message.replace("{Dewpoint}", String(int(pow((currentHumidity) / 100, 0.125) * (112 + 0.9 * (currentTemperature)) + 0.1 * (currentTemperature) - 112)));
    } else {
      message.replace("{TEXT_TEMPERATURE}: {Temperature} °C", "");
      message.replace("{TEXT_HUMIDITY}: {Humidity} %", "");
      message.replace("{TEXT_PRESSURE}: {Pressure} hPa", "");
      message.replace("{TEXT_DEWPOINT}: {Dewpoint} °C", "");
    }
  } else if (!strcmp(THP_MODEL, "DHT22")) {
    if (checkDHT22Status()) {
      message.replace("{TEXT_TEMPERATURE}", (TEXT_TEMPERATURE));
      message.replace("{TEXT_HUMIDITY}", (TEXT_HUMIDITY));
      message.replace("{TEXT_DEWPOINT}", (TEXT_DEWPOINT));

      message.replace("{Temperature}", String(int(currentTemperature)));
      message.replace("{TEXT_PRESSURE}: {Pressure} hPa", "");
      message.replace("{Humidity}", String(int(currentHumidity)));
      message.replace("{Dewpoint}", String(int(pow((currentHumidity) / 100, 0.125) * (112 + 0.9 * (currentTemperature)) + 0.1 * (currentTemperature) - 112)));
    } else {
      message.replace("{TEXT_TEMPERATURE}: {Temperature} °C", "");
      message.replace("{TEXT_HUMIDITY}: {Humidity} %", "");
      message.replace("{TEXT_PRESSURE}: {Pressure} hPa", "");
      message.replace("{TEXT_DEWPOINT}: {Dewpoint} °C", "");
    }
  } else if (!strcmp(THP_MODEL, "BMP280")) {
    if (checkBmpStatus()) {
      message.replace("{TEXT_TEMPERATURE}", (TEXT_TEMPERATURE));
      message.replace("{TEXT_PRESSURE}", (TEXT_PRESSURE));

      message.replace("{Temperature}", String(int(currentTemperature)));
      message.replace("{Pressure}", String(int(currentPressure)));
      message.replace("{TEXT_HUMIDITY}: {Humidity} %", "");
      message.replace("{TEXT_DEWPOINT}: {Pressure} °C", "");
    } else {
      message.replace("{TEXT_TEMPERATURE}: {Temperature} °C", "");
      message.replace("{TEXT_HUMIDITY}: {Humidity} %", "");
      message.replace("{TEXT_PRESSURE}: {Pressure} hPa", "");
      message.replace("{TEXT_DEWPOINT}: {Dewpoint} °C", "");
    }
  } else if (!strcmp(THP_MODEL, "SHT1x")) {
    if (checkSHT1xStatus()) {
      message.replace("{TEXT_TEMPERATURE}", (TEXT_TEMPERATURE));
      message.replace("{TEXT_HUMIDITY}", (TEXT_HUMIDITY));
      message.replace("{TEXT_DEWPOINT}", (TEXT_DEWPOINT));

      message.replace("{Temperature}", String(int(currentTemperature)));
      message.replace("{TEXT_PRESSURE}: {Pressure} hPa", "");
      message.replace("{Humidity}", String(float(currentHumidity)));
      message.replace("{Dewpoint}", String(int(pow((currentHumidity) / 100, 0.125) * (112 + 0.9 * (currentTemperature)) + 0.1 * (currentTemperature) - 112)));
    } else {
      message.replace("{TEXT_TEMPERATURE}: {Temperature} °C", "");
      message.replace("{TEXT_HUMIDITY}: {Humidity} %", "");
      message.replace("{TEXT_PRESSURE}: {Pressure} hPa", "");
      message.replace("{TEXT_DEWPOINT}: {Dewpoint} °C", "");
    }
  }

  if (strcmp(DUST_MODEL, "Non")) {
    message.replace("{TEXT_AIRPOLLUTION}", (TEXT_AIRPOLLUTION));

    if (DISPLAY_PM1) {
      message.replace("{averagePM1}", String(averagePM1));
    } else {
      message.replace("PM1: {averagePM1} µg/m³", "");
    }

    if (averagePM25 <= 10) {
      message.replace("{colorAveragePM25}", "<font color='#61EEE4'>");
    } else if (averagePM25 > 10 && averagePM25 <= 20) {
      message.replace("{colorAveragePM25}", "<font color='#5BCAAA'>");
    } else if (averagePM25 > 20 && averagePM25 <= 25) {
      message.replace("{colorAveragePM25}", "<font color='#EEE25D'>");
    } else if (averagePM25 > 25 && averagePM25 <= 50) {
      message.replace("{colorAveragePM25}", "<font color='#F95459'>");
    } else if (averagePM25 > 50) {
      message.replace("{colorAveragePM25}", "<font color='#920736'>");
    } else {
      message.replace("{colorAveragePM25}", "<font>");
    }

    message.replace("{averagePM25}", String(averagePM25) + "</font>");

    if (averagePM10 <= 20) {
      message.replace("{colorAveragePM10}", "<font color='#61EEE4'>");
    } else if (averagePM10 > 20 && averagePM10 <= 35) {
      message.replace("{colorAveragePM10}", "<font color='#5BCAAA'>");
    } else if (averagePM10 > 35 && averagePM10 <= 50) {
      message.replace("{colorAveragePM10}", "<font color='#EEE25D'>");
    } else if (averagePM10 > 50 && averagePM10 <= 100) {
      message.replace("{colorAveragePM10}", "<font color='#F95459'>");
    } else if (averagePM10 > 100) {
      message.replace("{colorAveragePM10}", "<font color='#920736'>");
    } else {
      message.replace("{colorAveragePM10}", "<font>");
    }
    message.replace("{averagePM10}", String(averagePM10) + "</font>");
  } else {
    message.replace("{TEXT_AIRPOLLUTION}:", "");
    message.replace("PM1: {averagePM1} µg/m³", "");
    message.replace("PM2.5: {colorAveragePM25} {averagePM25} µg/m³", "");
    message.replace("PM10: {colorAveragePM10} {averagePM10} µg/m³", "");
  }

  if (AIRMONITOR_GRAPH_ON) {
    message += FPSTR(WEB_ROOT_PAGE_AIRMONITOR_GRAPH);
    message.replace("{LATITUDE}", String(LATITUDE));
    message.replace("{LONGITUDE}", String(LONGITUDE));
  }

  if (THINGSPEAK_GRAPH_ON) {
    message += FPSTR(WEB_ROOT_PAGE_THINGSPEAK_GRAPH);
    message.replace("{THINGSPEAK_CHANNEL_ID}", String(THINGSPEAK_CHANNEL_ID));
	message.replace("{THINGSPEAK_READ_API_KEY}", String(THINGSPEAK_READ_API_KEY));
  }

  message += FPSTR(WEB_PAGE_FOOTER);
  request->send(200, "text/html", message);
  //WebServer.send(200, "text/html", message);
}

String _addOption(const String &value, const String &label, const String &srslyValue) {
  String option = FPSTR(WEB_CONFIG_PAGE_ADDOPTION);
  option.replace("{value}", value);
  if (value == srslyValue) {
    option.replace("{srslyValue}", " selected>");
  } else {
    option.replace("{srslyValue}", ">");
  }
  option.replace("{label}", label);
  return option;
}

String _addBoolSelect(const String &key, const bool &value) {
  String selectValue = value ? "yes" : "no";
  String input = FPSTR(WEB_CONFIG_PAGE_SELECT);
  input.replace("{key}", key);
  input += _addOption("yes", (TEXT_YES), selectValue);
  input += _addOption("no", (TEXT_NO), selectValue);
  input += FPSTR(WEB_CONFIG_PAGE_SELECTEND);
  return input;
}

String _addModelSelect(const String &key, const String &value) {
  String input = FPSTR(WEB_CONFIG_PAGE_SELECT);
  input.replace("{key}", key);
  input += _addOption("red", (TEXT_WITHOUTCALIBRATION), value);
  if (!strcmp(THP_MODEL, "BME280")) {
    input += _addOption("white", (TEXT_AUTOMATICCALIBRATION), value);
  }
  if (!strcmp(THP_MODEL, "HTU21")) {
    input += _addOption("white", (TEXT_AUTOMATICCALIBRATION), value);
  }
  if (!strcmp(THP_MODEL, "DHT22")) {
    input += _addOption("white", (TEXT_AUTOMATICCALIBRATION), value);
  }
  if (!strcmp(THP_MODEL, "SHT1x")) {
    input += _addOption("white", (TEXT_AUTOMATICCALIBRATION), value);
  }
  input += FPSTR(WEB_CONFIG_PAGE_SELECTEND);
  return input;
}

String _addTHP_MODELSelect(const String &key, const String &value) {
  String input = FPSTR(WEB_CONFIG_PAGE_SELECT);
  input.replace("{key}", key);
  input += _addOption("BME280", "BME280", value);
  if (strcmp(PMSENSORVERSION, "PMS-SparkFunBME280")) {
    if (!strcmp(DUST_MODEL, "PMS7003") or !strcmp(DUST_MODEL, "Non")) {
      input += _addOption("BME280-SparkFun", "BME280-SparkFun", value);
    }
  }
  input += _addOption("SHT1x", "SHT1x", value);
  input += _addOption("HTU21", "SHT21/HTU21D", value);
  input += _addOption("DHT22", "DHT22", value);
  input += _addOption("BMP280", "BMP280", value);

  input += _addOption("Non", (TEXT_WITHOUTSENSOR), value);
  input += FPSTR(WEB_CONFIG_PAGE_SELECTEND);
  return input;
}

String _addDUST_MODELSelect(const String &key, const String &value) {
  String input = FPSTR(WEB_CONFIG_PAGE_SELECT);
  input.replace("{key}", key);
  input += _addOption("PMS7003", "PMS5003/7003", value);
  input += _addOption("SDS011/21", "SDS011/21", value);
  input += _addOption("HPMA115S0", "HPMA115S0", value);
  input += _addOption("SPS30", "SPS30", value);

  input += _addOption("Non", (TEXT_WITHOUTSENSOR), value);
  input += FPSTR(WEB_CONFIG_PAGE_SELECTEND);
  return input;
}

String _addINFLUXDB_VERSIONSelect(const String &key, const String &value) {
  String input = FPSTR(WEB_CONFIG_PAGE_SELECT);
  input.replace("{key}", key);
  input += _addOption("1", "1.x", value);
  input += _addOption("2", "2.x", value);
  input += FPSTR(WEB_CONFIG_PAGE_SELECTEND);
  return input;
}

String _addLanguageSelect(const String &key, const String &value) {
  String input = FPSTR(WEB_CONFIG_PAGE_SELECT);
  input.replace("{key}", key);
  input += _addOption("polish", (TEXT_INTL_PL), value);
  input += _addOption("english", (TEXT_INTL_EN), value);
  input += FPSTR(WEB_CONFIG_PAGE_SELECTEND);
  return input;
}

String _escapeString (const String &value) {
  String trimmed = value;
  trimmed.trim();
  trimmed.replace("'", "&#39;");
  return trimmed;
}

String _addTextInput(const String &key, const String &value, const String &postfix = "") {
  String input = FPSTR(WEB_CONFIG_PAGE_TEXTIMPUT);
  input.replace("{key}", key);
  input.replace("{value}", _escapeString(value));
  input.replace("{postfix}", postfix);
  return input;
}

String _addMQTTTextInput(const String &key, const String &value, const String &postfix = "") {
  String input = FPSTR(WEB_CONFIG_PAGE_MQTT_TEXTIMPUT);
  input.replace("{key}", key);
  input.replace("{value}", _escapeString(value));
  input.replace("{postfix}", postfix);
  return input;
}

String _addPasswdInput(const String &key, const String &value, const String &postfix = "") {
  String input = FPSTR(WEB_CONFIG_PAGE_PASSWDINPUT);
  input.replace("{key}", key);
  input.replace("{value}", _escapeString(value));
  input.replace("{postfix}", postfix);
  return input;
}

String _addIntInput(const String &key, const int &value, const String &postfix = "") {
  String input = FPSTR(WEB_CONFIG_PAGE_INTINPUT);
  input.replace("{key}", key);
  input.replace("{value}", String(value));
  input.replace("{postfix}", postfix);
  return input;
}

String _addFloatInput(const String &key, const double &value, const int &precision = 6, const String &postfix = "") {
  String input = FPSTR(WEB_CONFIG_PAGE_FLOATINPUT);
  input.replace("{key}", key);
  input.replace("{value}", String(value, precision));
  input.replace("{postfix}", postfix);
  return input;
}

String _addSubmitDevice() {
  String submitDevice = FPSTR(WEB_CONFIG_PAGE_SUBMIT_DEVICE_BUTTON);
  submitDevice.replace("{TEXT_SAVE}", (TEXT_SAVE));
  return submitDevice;
}

String _addSubmitServices() {
  String submitServices = FPSTR(WEB_CONFIG_PAGE_SUBMIT_SERVICES_BUTTON);
  submitServices.replace("{TEXT_SAVE}", (TEXT_SAVE));
  return submitServices;
}

String _addWiFiErase() {
  String WiFiErase = FPSTR(WEB_CONFIG_PAGE_WIFIERASE);
  WiFiErase.replace("{TEXT_ERASEWIFICONFIG}", (TEXT_ERASEWIFICONFIG));
  return WiFiErase;
}

String _addRestoreConfig() {
  String RestoreConfig = FPSTR(WEB_CONFIG_PAGE_RESTORECONFIG);
  RestoreConfig.replace("{TEXT_RESTORESETTINGS}", (TEXT_RESTORESETTINGS));
  return RestoreConfig;
}

//void handle_config() {
void handle_config(AsyncWebServerRequest *request) {
  String message = FPSTR(WEB_PAGE_HEADER);
  message.replace("{WEB_PAGE_CSS}", FPSTR(WEB_PAGE_HEADER_CSS));
  message.replace("{Language}", (TEXT_LANG));
  message.replace("{CurrentPageTitle}", (TEXT_CONFIG_PAGE));
  message.replace("{IndexPageTitle}", (TEXT_INDEX_PAGE));
  message.replace("{ConfigPageTitle}", (TEXT_CONFIG_PAGE));
  message.replace("{UpdatePageTitle}", (TEXT_UPDATE_PAGE));

  message += FPSTR(WEB_CONFIG_PAGE_TOP);
  message.replace("{TEXT_CONFIG_PAGE}", (TEXT_CONFIG_PAGE));

  message += FPSTR(WEB_CONFIG_PAGE_CONFIG);
  message.replace("{TEXT_CONFIG_INFO_DEVICE_PAGE}", (TEXT_CONFIG_INFO_DEVICE_PAGE));
  message.replace("{TEXT_CONFIG_DEVICE_PAGE}", (TEXT_CONFIG_DEVICE_PAGE));

  message.replace("{TEXT_CONFIG_INFO_SERVICES_PAGE}", (TEXT_CONFIG_INFO_SERVICES_PAGE));
  message.replace("{TEXT_CONFIG_SERVICES_PAGE}", (TEXT_CONFIG_SERVICES_PAGE));

  message.replace("{ConfigDeviceButton}", FPSTR(WEB_CONFIG_DEVICE_PAGE_BUTTON));
  message.replace("{TEXT_CONFIG_DEVICE_PAGE}", (TEXT_CONFIG_DEVICE_PAGE));
  message.replace("{ConfigServicesButton}", FPSTR(WEB_CONFIG_SERVICES_PAGE_BUTTON));
  message.replace("{TEXT_CONFIG_SERVICES_PAGE}", (TEXT_CONFIG_SERVICES_PAGE));

  message.replace("{WiFiEraseButton}", _addWiFiErase());
  message.replace("{RestoreConfigButton}", _addRestoreConfig());

  message += FPSTR(WEB_PAGE_FOOTER);
request->send(200, "text/html", message);
  //WebServer.send(200, "text/html", message);
}

//void _handle_config_device(bool is_success) {
void handle_config_device(AsyncWebServerRequest *request) {
  if (CONFIG_AUTH == true) {
	  /*
	if (!WebServer.authenticate(CONFIG_USERNAME, CONFIG_PASSWORD)) {
      //return WebServer.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
      return WebServer.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
    }
	  */
	  if(!request->authenticate(CONFIG_USERNAME,CONFIG_PASSWORD,NULL,false)) {
	        request->requestAuthentication(NULL,false); // force basic auth
	      }
  }
  
#ifdef ARDUINO_ARCH_ESP8266
    String message = FPSTR(WEB_PAGE_HEADER);
  message.replace("{WEB_PAGE_CSS}", FPSTR(WEB_PAGE_HEADER_CSS));
  message.replace("{Language}", (TEXT_LANG));
  message.replace("{CurrentPageTitle}", (TEXT_CONFIG_PAGE));
  message.replace("{IndexPageTitle}", (TEXT_INDEX_PAGE));
  message.replace("{ConfigPageTitle}", (TEXT_CONFIG_PAGE));
  message.replace("{UpdatePageTitle}", (TEXT_UPDATE_PAGE));

  message += FPSTR(WEB_CONFIG_DEVICE_PAGE_TOP);

  message.replace("{TEXT_CONFIG_DEVICE_PAGE}", (TEXT_CONFIG_DEVICE_PAGE));

  message.replace("<div style='color: #2f7a2d'> <strong>{TEXT_SAVED}!</strong> - {TEXT_POSTCONFIG_INFO} </div><br><hr><br>", "");

  message.replace("{TEXT_INSTRUCIONSLINK}", (TEXT_INSTRUCIONSLINK));
  message.replace("{GITHUB_LINK}", String(GITHUB_LINK));
  message.replace("{TEXT_HERE}", (TEXT_HERE));
  
  message += FPSTR(WEB_CONFIG_DEVICE_PAGE_CONFIG);

  message.replace("{TEXT_DEVICENAME}", (TEXT_DEVICENAME));
  
  if (DEVICENAME_AUTO) {
    message.replace("{device_name}", (device_name));
  } else {
    message.replace("{device_name}", _addTextInput("DEVICENAME", DEVICENAME));
  }

  message.replace("{TEXT_DEVICENAMEAUTO}", (TEXT_DEVICENAMEAUTO));
  message.replace("{DEVICENAME_AUTO}", _addBoolSelect("DEVICENAME_AUTO", DEVICENAME_AUTO));
  message.replace("{TEXT_SELECTEDLANGUAGE}", (TEXT_SELECTEDLANGUAGE));
  message.replace("{LanguageSelect}", _addLanguageSelect("LANGUAGE", LANGUAGE));
  message.replace("{TEXT_TEMPHUMIPRESSSENSOR}", (TEXT_TEMPHUMIPRESSSENSOR));
  message.replace("{THP_MODELSelect}", _addTHP_MODELSelect("THP_MODEL", THP_MODEL));
  message.replace("{TEXT_PMSENSOR}", (TEXT_PMSENSOR));
  message.replace("{DUST_MODELSelect}", _addDUST_MODELSelect("DUST_MODEL", DUST_MODEL));

  message.replace("{TEXT_FREQUENTMEASUREMENTONOFF}", (TEXT_FREQUENTMEASUREMENTONOFF));
  message.replace("{TEXT_FREQUENTMEASUREMENTINFO}", (TEXT_FREQUENTMEASUREMENTINFO));
  message.replace("{TEXT_MEASUREMENTFREQUENCY}", (TEXT_MEASUREMENTFREQUENCY));
  
  message.replace("{FREQUENTMEASUREMENT_Select}", _addBoolSelect("FREQUENTMEASUREMENT", FREQUENTMEASUREMENT));
  
  if (FREQUENTMEASUREMENT == true) {
    message.replace("{FREQUENTMEASUREMENT_time}", _addIntInput("DUST_TIME", DUST_TIME, "{TEXT_SECONDS}"));
    message.replace("{TEXT_SECONDS}", (TEXT_SECONDS));
  } else {
    message.replace("{FREQUENTMEASUREMENT_time}", _addIntInput("DUST_TIME", DUST_TIME, "{TEXT_MINUTES}"));
    message.replace("{TEXT_MINUTES}", (TEXT_MINUTES));
  }

  message.replace("{TEXT_AVERAGELASTRESULT}", (TEXT_AVERAGELASTRESULT));
  message.replace("{NUMBEROFMEASUREMENTS}", _addIntInput("NUMBEROFMEASUREMENTS", NUMBEROFMEASUREMENTS, "{TEXT_PMMEASUREMENTS}"));
  message.replace("{TEXT_PMMEASUREMENTS}", (TEXT_PMMEASUREMENTS));

  if (FREQUENTMEASUREMENT == true) {
    message.replace("<hr><b>DeepSleep: </b>{DEEPSLEEP_ON} {TEXT_DEEPSLEEPINFO}", "");
  } else {
#ifdef ARDUINO_ARCH_ESP8266
    message.replace("{TEXT_DEEPSLEEPINFO}", TEXT_DEEPSLEEPINFO);
#elif defined ARDUINO_ARCH_ESP32
    message.replace("{TEXT_DEEPSLEEPINFO}", TEXT_DEEPSLEEPINFO_ESP32);
#endif
    message.replace("{INTERFACEWWWONTIME}", String(int(NUMBEROFMEASUREMENTS) * 2 + 10 ));
    message.replace("{SENDING_FREQUENCY}", String(SENDING_FREQUENCY));
    message.replace("{DEEPSLEEP_ON}", _addBoolSelect("DEEPSLEEP_ON", DEEPSLEEP_ON));
  }

  if (!strcmp(DUST_MODEL, "PMS7003") or !strcmp(DUST_MODEL, "SPS30")) {
	message.replace("{DISPLAY_PM1}", _addBoolSelect("DISPLAY_PM1", DISPLAY_PM1)); // ERROR
    message.replace("{TEXT_DISPLAYPM1}", (TEXT_DISPLAYPM1));
  } else {
    message.replace("<b>{TEXT_DISPLAYPM1}: </b> {DISPLAY_PM1}", "");
  }

  message.replace("{TEXT_ALTITUDEINFO}", (TEXT_ALTITUDEINFO));
  message.replace("{WSPOLRZEDNE_GPS_LINK}", String(WSPOLRZEDNE_GPS_LINK));
  message.replace("{TEXT_HERE}", (TEXT_HERE));
  message.replace("{MYALTITUDE}", _addIntInput("MYALTITUDE", MYALTITUDE, "m.n.p.m"));

  message.replace("{TEXT_SECURECONFIGUPDATEPAGE}", (TEXT_SECURECONFIGUPDATEPAGE));
  message.replace("{CONFIG_AUTH}", _addBoolSelect("CONFIG_AUTH", CONFIG_AUTH));
  message.replace("{TEXT_SECURELOGIN}", (TEXT_SECURELOGIN));

  message.replace("{CONFIG_USERNAME}", _addTextInput("CONFIG_USERNAME", CONFIG_USERNAME));
  message.replace("{TEXT_SECUREPASSWD}", (TEXT_SECUREPASSWD));
  message.replace("{CONFIG_PASSWORD}", _addPasswdInput("CONFIG_PASSWORD", CONFIG_PASSWORD));
  /*
  if (!CONFIG_AUTH) {
    message.replace("{TEXT_SECURELOGOUTINFO}", "");
  } else {
    message.replace("{TEXT_SECURELOGOUTINFO}", (TEXT_SECURELOGOUTINFO));
  }
  */
  message.replace("{DEBUG}", _addBoolSelect("DEBUG", DEBUG));
  message.replace("{TEXT_CALIBMETHOD}", (TEXT_CALIBMETHOD));
  message.replace("{CalibrationModelSelect}", _addModelSelect("MODEL", MODEL));
  /*
  message.replace("{TEXT_CALIB1}", (TEXT_CALIB1));
  message.replace("{calib1}", String(float(calib1)));
  
  message.replace("{TEXT_CALIB2}", (TEXT_CALIB2));
  message.replace("{calib2}", String(float(calib2)));
  
  message.replace("{TEXT_SOFTWATEVERSION}", (TEXT_SOFTWATEVERSION));
  message.replace("{SOFTWAREVERSION}", SOFTWAREVERSION);

  message.replace("{TEXT_AUTOUPDATEON}", TEXT_AUTOUPDATEON);
  message.replace("{AUTOUPDATEON}", _addBoolSelect("AUTOUPDATE_ON", AUTOUPDATE_ON));

#ifdef ARDUINO_ARCH_ESP8266
  message.replace("{TEXT_UPDATEPAGEAUTOUPDATEWARNING}", TEXT_UPDATEPAGEAUTOUPDATEWARNING);
#elif defined ARDUINO_ARCH_ESP32
  message.replace("{TEXT_UPDATEPAGEAUTOUPDATEWARNING}", "");
#endif
*/
  message.replace("{WiFiEraseButton}", _addWiFiErase());
  message.replace("{RestoreConfigButton}", _addRestoreConfig());
  message.replace("{SubmitButton}", _addSubmitDevice());
  message += FPSTR(WEB_PAGE_FOOTER);

#elif defined ARDUINO_ARCH_ESP32
    String message = FPSTR(WEB_PAGE_HEADER);
  message.replace("{WEB_PAGE_CSS}", FPSTR(WEB_PAGE_HEADER_CSS));
  message.replace("{Language}", (TEXT_LANG));
  message.replace("{CurrentPageTitle}", (TEXT_CONFIG_PAGE));
  message.replace("{IndexPageTitle}", (TEXT_INDEX_PAGE));
  message.replace("{ConfigPageTitle}", (TEXT_CONFIG_PAGE));
  message.replace("{UpdatePageTitle}", (TEXT_UPDATE_PAGE));

  message += FPSTR(WEB_CONFIG_DEVICE_PAGE_TOP);

  message.replace("{TEXT_CONFIG_DEVICE_PAGE}", (TEXT_CONFIG_DEVICE_PAGE));
  /*
  if (!is_success) {
    message.replace("<div style='color: #2f7a2d'> <strong>{TEXT_SAVED}!</strong> - {TEXT_POSTCONFIG_INFO} </div><br><hr><br>", "");
  } else {
    message.replace("{TEXT_SAVED}", (TEXT_SAVED));
    message.replace("{TEXT_POSTCONFIG_INFO}", (TEXT_POSTCONFIG_INFO));
  }
*/
  message.replace("<div style='color: #2f7a2d'> <strong>{TEXT_SAVED}!</strong> - {TEXT_POSTCONFIG_INFO} </div><br><hr><br>", "");
  
  message.replace("{TEXT_INSTRUCIONSLINK}", (TEXT_INSTRUCIONSLINK));
  message.replace("{GITHUB_LINK}", String(GITHUB_LINK));
  message.replace("{TEXT_HERE}", (TEXT_HERE));
 
  message += FPSTR(WEB_CONFIG_DEVICE_PAGE_CONFIG);

  message.replace("{TEXT_DEVICENAME}", (TEXT_DEVICENAME));
  
  if (DEVICENAME_AUTO) {
    message.replace("{device_name}", (device_name));
  } else {
    message.replace("{device_name}", _addTextInput("DEVICENAME", DEVICENAME));
  }
  
  message.replace("{TEXT_DEVICENAMEAUTO}", (TEXT_DEVICENAMEAUTO));
  message.replace("{DEVICENAME_AUTO}", _addBoolSelect("DEVICENAME_AUTO", DEVICENAME_AUTO));
  message.replace("{TEXT_SELECTEDLANGUAGE}", (TEXT_SELECTEDLANGUAGE));
  message.replace("{LanguageSelect}", _addLanguageSelect("LANGUAGE", LANGUAGE));
  message.replace("{TEXT_TEMPHUMIPRESSSENSOR}", (TEXT_TEMPHUMIPRESSSENSOR));
  message.replace("{THP_MODELSelect}", _addTHP_MODELSelect("THP_MODEL", THP_MODEL));
  message.replace("{TEXT_PMSENSOR}", (TEXT_PMSENSOR));
  message.replace("{DUST_MODELSelect}", _addDUST_MODELSelect("DUST_MODEL", DUST_MODEL));

  message.replace("{TEXT_FREQUENTMEASUREMENTONOFF}", (TEXT_FREQUENTMEASUREMENTONOFF));
  message.replace("{TEXT_FREQUENTMEASUREMENTINFO}", (TEXT_FREQUENTMEASUREMENTINFO));
  message.replace("{TEXT_MEASUREMENTFREQUENCY}", (TEXT_MEASUREMENTFREQUENCY));
  
  message.replace("{FREQUENTMEASUREMENT_Select}", _addBoolSelect("FREQUENTMEASUREMENT", FREQUENTMEASUREMENT));
  
  if (FREQUENTMEASUREMENT == true) {
    message.replace("{FREQUENTMEASUREMENT_time}", _addIntInput("DUST_TIME", DUST_TIME, "{TEXT_SECONDS}"));
    message.replace("{TEXT_SECONDS}", (TEXT_SECONDS));
  } else {
    message.replace("{FREQUENTMEASUREMENT_time}", _addIntInput("DUST_TIME", DUST_TIME, "{TEXT_MINUTES}"));
    message.replace("{TEXT_MINUTES}", (TEXT_MINUTES));
  }

  message.replace("{TEXT_AVERAGELASTRESULT}", (TEXT_AVERAGELASTRESULT));
  message.replace("{NUMBEROFMEASUREMENTS}", _addIntInput("NUMBEROFMEASUREMENTS", NUMBEROFMEASUREMENTS, "{TEXT_PMMEASUREMENTS}"));
  message.replace("{TEXT_PMMEASUREMENTS}", (TEXT_PMMEASUREMENTS));

  if (FREQUENTMEASUREMENT == true) {
    message.replace("<hr><b>DeepSleep: </b>{DEEPSLEEP_ON} {TEXT_DEEPSLEEPINFO}", "");
  } else {
#ifdef ARDUINO_ARCH_ESP8266
    message.replace("{TEXT_DEEPSLEEPINFO}", TEXT_DEEPSLEEPINFO);
#elif defined ARDUINO_ARCH_ESP32
    message.replace("{TEXT_DEEPSLEEPINFO}", TEXT_DEEPSLEEPINFO_ESP32);
#endif
    message.replace("{INTERFACEWWWONTIME}", String(int(NUMBEROFMEASUREMENTS) * 2 + 10 ));
    message.replace("{SENDING_FREQUENCY}", String(SENDING_FREQUENCY));
    message.replace("{DEEPSLEEP_ON}", _addBoolSelect("DEEPSLEEP_ON", DEEPSLEEP_ON));
  }
  
  if (!strcmp(DUST_MODEL, "PMS7003") or !strcmp(DUST_MODEL, "SPS30")) {
	message.replace("{DISPLAY_PM1}", _addBoolSelect("DISPLAY_PM1", DISPLAY_PM1)); // ERROR
    message.replace("{TEXT_DISPLAYPM1}", (TEXT_DISPLAYPM1));
  } else {
    message.replace("<b>{TEXT_DISPLAYPM1}: </b> {DISPLAY_PM1}", "");
  }

  message.replace("{TEXT_ALTITUDEINFO}", (TEXT_ALTITUDEINFO));
  message.replace("{WSPOLRZEDNE_GPS_LINK}", String(WSPOLRZEDNE_GPS_LINK));
  message.replace("{TEXT_HERE}", (TEXT_HERE));
  message.replace("{MYALTITUDE}", _addIntInput("MYALTITUDE", MYALTITUDE, "m.n.p.m"));

  message.replace("{TEXT_SECURECONFIGUPDATEPAGE}", (TEXT_SECURECONFIGUPDATEPAGE));
  message.replace("{CONFIG_AUTH}", _addBoolSelect("CONFIG_AUTH", CONFIG_AUTH));
  message.replace("{TEXT_SECURELOGIN}", (TEXT_SECURELOGIN));
  
  message.replace("{CONFIG_USERNAME}", _addTextInput("CONFIG_USERNAME", CONFIG_USERNAME));
  message.replace("{TEXT_SECUREPASSWD}", (TEXT_SECUREPASSWD));
  message.replace("{CONFIG_PASSWORD}", _addPasswdInput("CONFIG_PASSWORD", CONFIG_PASSWORD));

  if (!CONFIG_AUTH) {
    message.replace("{TEXT_SECURELOGOUTINFO}", "");
  } else {
    message.replace("{TEXT_SECURELOGOUTINFO}", (TEXT_SECURELOGOUTINFO));
  }
  
  message.replace("{DEBUG}", _addBoolSelect("DEBUG", DEBUG));
  message.replace("{TEXT_CALIBMETHOD}", (TEXT_CALIBMETHOD));
  message.replace("{CalibrationModelSelect}", _addModelSelect("MODEL", MODEL));
  message.replace("{TEXT_CALIB1}", (TEXT_CALIB1));
  message.replace("{calib1}", String(calib1));
  
  message.replace("{TEXT_CALIB2}", (TEXT_CALIB2));
  message.replace("{calib2}", String(calib2));

  message.replace("{TEXT_SOFTWATEVERSION}", (TEXT_SOFTWATEVERSION));
  message.replace("{SOFTWAREVERSION}", SOFTWAREVERSION);

  message.replace("{TEXT_AUTOUPDATEON}", TEXT_AUTOUPDATEON);
  message.replace("{AUTOUPDATEON}", _addBoolSelect("AUTOUPDATE_ON", AUTOUPDATE_ON));
  
  
#ifdef ARDUINO_ARCH_ESP8266
  message.replace("{TEXT_UPDATEPAGEAUTOUPDATEWARNING}", TEXT_UPDATEPAGEAUTOUPDATEWARNING);
#elif defined ARDUINO_ARCH_ESP32
  message.replace("{TEXT_UPDATEPAGEAUTOUPDATEWARNING}", "");
#endif

  message.replace("{WiFiEraseButton}", _addWiFiErase());
  message.replace("{RestoreConfigButton}", _addRestoreConfig());
  message.replace("{SubmitButton}", _addSubmitDevice());
  message += FPSTR(WEB_PAGE_FOOTER);
#endif
  //WebServer.send(200, "text/html", message);
  request->send(200, "text/html", message);
}

//void _handle_config_services(bool is_success) {
void handle_config_services(AsyncWebServerRequest *request) {
	  if (CONFIG_AUTH == true) {
		  /*
		if (!WebServer.authenticate(CONFIG_USERNAME, CONFIG_PASSWORD)) {
	      //return WebServer.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
	      return WebServer.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
	    }
		  */
		  if(!request->authenticate(CONFIG_USERNAME,CONFIG_PASSWORD,NULL,false)) {
		        request->requestAuthentication(NULL,false); // force basic auth
		      }
	  }
#ifdef ARDUINO_ARCH_ESP8266
	    String message = FPSTR(WEB_PAGE_HEADER);
  message.replace("{WEB_PAGE_CSS}", FPSTR(WEB_PAGE_HEADER_CSS));
	  message.replace("{Language}", (TEXT_LANG));
	  message.replace("{CurrentPageTitle}", (TEXT_CONFIG_PAGE));
	  message.replace("{IndexPageTitle}", (TEXT_INDEX_PAGE));
	  message.replace("{ConfigPageTitle}", (TEXT_CONFIG_PAGE));
	  message.replace("{UpdatePageTitle}", (TEXT_UPDATE_PAGE));

	  message += FPSTR(WEB_CONFIG_SERVICES_PAGE_TOP);
	  message.replace("{TEXT_CONFIG_SERVICES_PAGE}", (TEXT_CONFIG_SERVICES_PAGE));  
  
	  message.replace("<div style='color: #2f7a2d'> <strong>{TEXT_SAVED}!</strong> - {TEXT_POSTCONFIG_INFO} </div><br><hr><br>", "");
  
	  message += FPSTR(WEB_CONFIG_SERVICES_PAGE_CONFIG);
  
	  message.replace("{WiFiEraseButton}", _addWiFiErase());
	  message.replace("{RestoreConfigButton}", _addRestoreConfig());
	  message.replace("{SubmitButton}", _addSubmitServices());
	  message += FPSTR(WEB_PAGE_FOOTER);
#elif defined ARDUINO_ARCH_ESP32
	    String message = FPSTR(WEB_PAGE_HEADER);
  message.replace("{WEB_PAGE_CSS}", FPSTR(WEB_PAGE_HEADER_CSS));
	  message.replace("{Language}", (TEXT_LANG));
	  message.replace("{CurrentPageTitle}", (TEXT_CONFIG_PAGE));
	  message.replace("{IndexPageTitle}", (TEXT_INDEX_PAGE));
	  message.replace("{ConfigPageTitle}", (TEXT_CONFIG_PAGE));
	  message.replace("{UpdatePageTitle}", (TEXT_UPDATE_PAGE));

	  message += FPSTR(WEB_CONFIG_SERVICES_PAGE_TOP);
	  message.replace("{TEXT_CONFIG_SERVICES_PAGE}", (TEXT_CONFIG_SERVICES_PAGE));  
	  /*
	  if (!is_success) {
	    message.replace("<div style='color: #2f7a2d'> <strong>{TEXT_SAVED}!</strong> - {TEXT_POSTCONFIG_INFO} </div><br><hr><br>", "");
	  } else {
	    message.replace("{TEXT_SAVED}", (TEXT_SAVED));
	    message.replace("{TEXT_POSTCONFIG_INFO}", (TEXT_POSTCONFIG_INFO));
	  }
	  */
  
	  message.replace("<div style='color: #2f7a2d'> <strong>{TEXT_SAVED}!</strong> - {TEXT_POSTCONFIG_INFO} </div><br><hr><br>", "");
  
	  message.replace("{TEXT_INSTRUCIONSLINK}", (TEXT_INSTRUCIONSLINK));
	  message.replace("{GITHUB_LINK}", String(GITHUB_LINK));
	  message.replace("{TEXT_HERE}", (TEXT_HERE));
  
	  message += FPSTR(WEB_CONFIG_SERVICES_PAGE_CONFIG);
  
	  if (FREQUENTMEASUREMENT == true) {
	    message.replace("{TEXT_SENDINGINTERVAL}", (TEXT_SERVICESSENDINGINTERVAL));
	    message.replace("{SENDING_FREQUENCY}", _addIntInput("SENDING_FREQUENCY", SENDING_FREQUENCY, "{TEXT_SECONDS}"));
	    message.replace("{TEXT_SECONDS}", (TEXT_SECONDS));

	    message.replace("{TEXT_DBSENDINGINTERVAL}", (TEXT_DBSENDINGINTERVAL));
	    message.replace("{SENDING_DB_FREQUENCY}", _addIntInput("SENDING_DB_FREQUENCY", SENDING_DB_FREQUENCY, "{TEXT_SECONDS}"));
	    message.replace("{TEXT_SECONDS}", (TEXT_SECONDS));
	  } else {
	    if (DEEPSLEEP_ON == true) {
	      message.replace("{TEXT_SENDINGINTERVAL}", (TEXT_SENDINGINTERVAL));
	      message.replace("{SENDING_FREQUENCY}", _addIntInput("SENDING_FREQUENCY", SENDING_FREQUENCY, "{TEXT_MINUTES}"));
	      message.replace("{TEXT_MINUTES}", (TEXT_MINUTES));

	      message.replace("<b>{TEXT_DBSENDINGINTERVAL}: </b>{SENDING_DB_FREQUENCY}", "");
	    } else {
	      message.replace("{TEXT_SENDINGINTERVAL}", (TEXT_SERVICESSENDINGINTERVAL));
	      message.replace("{SENDING_FREQUENCY}", _addIntInput("SENDING_FREQUENCY", SENDING_FREQUENCY, "{TEXT_MINUTES}"));
	      message.replace("{TEXT_MINUTES}", (TEXT_MINUTES));

	      message.replace("{TEXT_DBSENDINGINTERVAL}", (TEXT_DBSENDINGINTERVAL));
	      message.replace("{SENDING_DB_FREQUENCY}", _addIntInput("SENDING_DB_FREQUENCY", SENDING_DB_FREQUENCY, "{TEXT_MINUTES}"));
	      message.replace("{TEXT_MINUTES}", (TEXT_MINUTES));
	    }
	}
   
	  message.replace("{TEXT_SMOGLISTSENDING}", (TEXT_SMOGLISTSENDING));
	  message.replace("{SMOGLIST_LINK}", String(SMOGLIST_LINK));
	  message.replace("{SMOGLIST_ON}", _addBoolSelect("SMOGLIST_ON", SMOGLIST_ON));
	  message.replace("{TEXT_SMOGLISTINFO}", (TEXT_SMOGLISTINFO));

	  message.replace("{TEXT_LUFTDATENSENDING}", (TEXT_LUFTDATENSENDING));
	  message.replace("{LUFTDATEN_LINK}", (LUFTDATEN_LINK));
	  message.replace("{LUFTDATENFORM_LINK}", (LUFTDATENFORM_LINK));
	  message.replace("{LUFTDATEN_ON}", _addBoolSelect("LUFTDATEN_ON", LUFTDATEN_ON));
  
	#ifdef ARDUINO_ARCH_ESP8266
	  message.replace("{LUFTDATEN_ChipID}", "smogomierz-" + String(ESP.getChipId()));
	#elif defined ARDUINO_ARCH_ESP32
	  //message.replace("{ChipID}", "smogomierz-" + (ESP.getEfuseMac()));
	  message.replace("{LUFTDATEN_ChipID}", "smogomierz-" + String((uint32_t)(ESP.getEfuseMac())));  
	#endif
    
	  if (!strcmp(THP_MODEL, "BME280")) {
	    message.replace("{THPSENSOR}", "BME280");
	    message.replace("{THPXPIN}", "11");
	  } else if (!strcmp(THP_MODEL, "BMP280")) {
	    message.replace("{THPSENSOR}", "BMP280");
	    message.replace("{THPXPIN}", "3");
	  } else if (!strcmp(THP_MODEL, "HTU21")) {
	    message.replace("{THPSENSOR}", "HTU21");
	    message.replace("{THPXPIN}", "7");
	  } else if (!strcmp(THP_MODEL, "DHT22")) {
	    message.replace("{THPSENSOR}", "DHT22");
	    message.replace("{THPXPIN}", "7");
	  } else if (!strcmp(THP_MODEL, "SHT1x")) {
	    message.replace("{THPSENSOR}", "SHT1x");
	    message.replace("{THPXPIN}", "12");
	  } else {
	    message.replace("<br><b>{THPSENSOR}</b> Sensor PIN: <b>{THPXPIN}</b>", "");
	  }

	  if (!strcmp(DUST_MODEL, "PMS7003")) {
	    message.replace("{DUSTSENSOR}", "PMS5003/7003");
	    message.replace("{DUSTXPIN}", "1");
	  } else if (!strcmp(DUST_MODEL, "SDS011/21")) {
	    message.replace("{DUSTSENSOR}", "SDS011/21");
	    message.replace("{DUSTXPIN}", "1");
	  } else if (!strcmp(DUST_MODEL, "HPMA115S0")) {
	    message.replace("{DUSTSENSOR}", "HPMA115S0");
	    message.replace("{DUSTXPIN}", "1");
	  } else if (!strcmp(DUST_MODEL, "SPS30")) {
	    message.replace("{DUSTSENSOR}", "SPS30");
	    message.replace("{DUSTXPIN}", "1");
	  } else {
	    message.replace("<br><b>{DUSTSENSOR}</b> Sensor PIN: <b>{DUSTXPIN}</b>", "");
	  }

	  message.replace("{TEXT_AQIECOSENDING}", (TEXT_AQIECOSENDING));
	  message.replace("{AQI_ECO_ON}", _addBoolSelect("AQI_ECO_ON", AQI_ECO_ON));
	  message.replace("{TEXT_AQIECOSERVER}", (TEXT_AQIECOSERVER));
	  message.replace("{AQI_ECO_HOST}", _addTextInput("AQI_ECO_HOST", AQI_ECO_HOST));
	  message.replace("{TEXT_AQIECOPATH}", (TEXT_AQIECOPATH));
	  message.replace("{AQI_ECO_PATH}", _addTextInput("AQI_ECO_PATH", AQI_ECO_PATH));

	#ifdef ARDUINO_ARCH_ESP8266
	  message.replace("{ESP_MODEL}", "ESP8266");
	  message.replace("{AQI_ECO_ChipID}", String(ESP.getChipId()));
	#elif defined ARDUINO_ARCH_ESP32
	  //message.replace("{ChipID}", "smogomierz-" + (ESP.getEfuseMac()));
	  message.replace("{ESP_MODEL}", "ESP32");
	  message.replace("{AQI_ECO_ChipID}", String((uint32_t)(ESP.getEfuseMac())));  
	#endif

	  message.replace("{TEXT_AIRMONITORSENDING}", (TEXT_AIRMONITORSENDING));
  
	  char PMSENSORMODEL[16];
	  if (!strcmp(DUST_MODEL, "PMS7003") or !strcmp(DUST_MODEL, "Non")) {
	    strcpy(PMSENSORMODEL, "PMS7003");
	  } else if (!strcmp(DUST_MODEL, "SDS011/21")) {
	    strcpy(PMSENSORMODEL, "SDS011");
	  } else if (!strcmp(DUST_MODEL, "HPMA115S0")) {
	    strcpy(PMSENSORMODEL, "HPMA115S0");
	  } else if (!strcmp(DUST_MODEL, "SPS30")) {
	    strcpy(PMSENSORMODEL, "SPS30");
	  }
	  message.replace("{PMSENSORMODEL}", PMSENSORMODEL);

	  message.replace("{AIRMONITOR_LINK}", (AIRMONITOR_LINK));
	  message.replace("{AIRMONITORFORM_LINK}", (AIRMONITORFORM_LINK));
	  message.replace("{TEXT_THEFORM}", (TEXT_THEFORM));
	  message.replace("{TEXT_AIRMONITORCHARTS}", (TEXT_AIRMONITORCHARTS));
	  message.replace("{AIRMONITOR_ON}", _addBoolSelect("AIRMONITOR_ON", AIRMONITOR_ON));
  
	  message.replace("{TEXT_AIRMONITORCOORDINATESINFO}", (TEXT_AIRMONITORCOORDINATESINFO));
	  message.replace("{LATLONG_LINK}", (LATLONG_LINK));
	  message.replace("{TEXT_HERE}", (TEXT_HERE));
  
	  message.replace("{AIRMONITOR_GRAPH_ON}", _addBoolSelect("AIRMONITOR_GRAPH_ON", AIRMONITOR_GRAPH_ON));
	  message.replace("{TEXT_AIRMONITORLATITUDE}", (TEXT_AIRMONITORLATITUDE));
  
	  message.replace("{LATITUDE}", _addFloatInput("LATITUDE", atof(LATITUDE), 6, "°"));
	  message.replace("{TEXT_AIRMONITORLONGITUDE}", (TEXT_AIRMONITORLONGITUDE));
	  message.replace("{LONGITUDE}", _addFloatInput("LONGITUDE", atof(LONGITUDE), 6, "°"));
  
	  message.replace("{TEXT_THINGSPEAKSENDING}", (TEXT_THINGSPEAKSENDING));
	  message.replace("{THINGSPEAK_LINK}", (THINGSPEAK_LINK));
	  message.replace("{THINGSPEAK_ON}", _addBoolSelect("THINGSPEAK_ON", THINGSPEAK_ON));
	  message.replace("{TEXT_THINGSPEAKCHARTS}", (TEXT_THINGSPEAKCHARTS));
	  message.replace("{THINGSPEAK_GRAPH_ON}", _addBoolSelect("THINGSPEAK_GRAPH_ON", THINGSPEAK_GRAPH_ON));
	  message.replace("{TEXT_THINGSPEAK_WRITE_API_KEY}", (TEXT_THINGSPEAK_WRITE_API_KEY));
	  message.replace("{THINGSPEAK_API_KEY}", _addTextInput("THINGSPEAK_API_KEY", THINGSPEAK_API_KEY));
	  message.replace("{TEXT_THINGSPEAKCHANNELID}", (TEXT_THINGSPEAKCHANNELID));
	  message.replace("{THINGSPEAK_CHANNEL_ID}", _addIntInput("THINGSPEAK_CHANNEL_ID", THINGSPEAK_CHANNEL_ID));
	  message.replace("{TEXT_THINGSPEAK_READ_API_KEY}", (TEXT_THINGSPEAK_READ_API_KEY));
	  message.replace("{THINGSPEAK_READ_API_KEY}", _addTextInput("THINGSPEAK_READ_API_KEY", THINGSPEAK_READ_API_KEY));
  
	  message.replace("{TEXT_INFLUXDBSENDING}", (TEXT_INFLUXDBSENDING));
	  message.replace("{INFLUXDB_ON}", _addBoolSelect("INFLUXDB_ON", INFLUXDB_ON));
	  message.replace("{TEXT_INFLUXDBVERSION}", (TEXT_INFLUXDBVERSION));
	  message.replace("{INFLUXDB_VERSION}", _addINFLUXDB_VERSIONSelect("INFLUXDB_VERSION", INFLUXDB_VERSION));
	  message.replace("{TEXT_INFLUXDBSERVER}", (TEXT_INFLUXDBSERVER));
	  message.replace("{INFLUXDB_HOST}", _addTextInput("INFLUXDB_HOST", INFLUXDB_HOST));
	  message.replace("{TEXT_INFLUXDBPORT}", (TEXT_INFLUXDBPORT));
	  message.replace("{INFLUXDB_PORT}", _addIntInput("INFLUXDB_PORT", INFLUXDB_PORT));
	  message.replace("{TEXT_INFLUXDBNAME}", (TEXT_INFLUXDBNAME));
	  message.replace("{INFLUXDB_DATABASE}", _addTextInput("INFLUXDB_DATABASE", INFLUXDB_DATABASE));
	  message.replace("{TEXT_INFLUXDBUSER}", (TEXT_INFLUXDBUSER));
	  message.replace("{INFLUXDB_USER}", _addTextInput("INFLUXDB_USER", INFLUXDB_USER));
	  message.replace("{TEXT_INFLUXDBPASSWD}", (TEXT_INFLUXDBPASSWD));
	  message.replace("{INFLUXDB_PASSWORD}", _addPasswdInput("INFLUXDB_PASSWORD", INFLUXDB_PASSWORD));
  
	  if (!strcmp(INFLUXDB_VERSION, "2")) {
	  message.replace("{TEXT_INFLUXDBORG}", (TEXT_INFLUXDBORG));
	  message.replace("{INFLUXDB_ORG}", _addTextInput("INFLUXDB_ORG", INFLUXDB_ORG));
	  message.replace("{TEXT_INFLUXDBBUCKET}", (TEXT_INFLUXDBBUCKET));
	  message.replace("{INFLUXDB_BUCKET}", _addTextInput("INFLUXDB_BUCKET", INFLUXDB_BUCKET));
	  message.replace("{TEXT_INFLUXDBTOKEN}", (TEXT_INFLUXDBTOKEN));
	  message.replace("{INFLUXDB_TOKEN}", _addTextInput("INFLUXDB_TOKEN", INFLUXDB_TOKEN));
	  } else {
		  message.replace("<b>{TEXT_INFLUXDBORG}: </b>{INFLUXDB_ORG}", "");
		  message.replace("<b>{TEXT_INFLUXDBBUCKET}: </b>{INFLUXDB_BUCKET}", "");
		  message.replace("<b>{TEXT_INFLUXDBTOKEN}: </b>{INFLUXDB_TOKEN}", "");
	  }
	  
	  message.replace("{TEXT_MQTTSENDING}", (TEXT_MQTTSENDING));
	  message.replace("{MQTT_ON}", _addBoolSelect("MQTT_ON", MQTT_ON));
	  message.replace("{TEXT_MQTTSERVER}", (TEXT_MQTTSERVER));
	  message.replace("{MQTT_HOST}", _addTextInput("MQTT_HOST", MQTT_HOST));
	  message.replace("{TEXT_MQTTPORT}", (TEXT_MQTTPORT));
	  message.replace("{MQTT_PORT}", _addIntInput("MQTT_PORT", MQTT_PORT));
	  message.replace("{TEXT_MQTTUSER}", (TEXT_MQTTUSER));
	  message.replace("{MQTT_USER}", _addTextInput("MQTT_USER", MQTT_USER));
	  message.replace("{TEXT_MQTTPASSWD}", (TEXT_MQTTPASSWD));
	  message.replace("{MQTT_PASSWORD}", _addPasswdInput("MQTT_PASSWORD", MQTT_PASSWORD));

	  message.replace("{TEXT_MQTT_TOPIC_INFO}", (TEXT_MQTT_TOPIC_INFO));
  
	  message.replace("{TEXT_MQTT_IP_IN_TOPIC}", (TEXT_MQTT_IP_IN_TOPIC));
	  message.replace("{MQTT_IP_IN_TOPIC}", _addBoolSelect("MQTT_IP_IN_TOPIC", MQTT_IP_IN_TOPIC));
  
	  message.replace("{TEXT_MQTT_DEVICENAME_IN_TOPIC}", (TEXT_MQTT_DEVICENAME_IN_TOPIC));
	  message.replace("{MQTT_DEVICENAME_IN_TOPIC}", _addBoolSelect("MQTT_DEVICENAME_IN_TOPIC", MQTT_DEVICENAME_IN_TOPIC));
    
	  if (strcmp(THP_MODEL, "Non")) {
	    takeTHPMeasurements();
	    if (!strcmp(THP_MODEL, "BME280")) {
	      if (checkBmeStatus() == true) {
		      message.replace("{MQTT_TEMP}", String(int(currentTemperature)));
		  	  message.replace("{MQTT_TOPIC_TEMP}", _addMQTTTextInput("MQTT_TOPIC_TEMP", MQTT_TOPIC_TEMP));
	
		      message.replace("{MQTT_HUMI}", String(int(currentHumidity)));
		      message.replace("{MQTT_TOPIC_HUMI}", _addMQTTTextInput("MQTT_TOPIC_HUMI", MQTT_TOPIC_HUMI));
	
		      message.replace("{MQTT_PRESS}", String(int(currentPressure)));
		      message.replace("{MQTT_TOPIC_PRESS}", _addMQTTTextInput("MQTT_TOPIC_PRESS", MQTT_TOPIC_PRESS));
	      } else {
	        if (DEBUG) {
	          Serial.println("No measurements from BME280!\n");
	        }
	      }
	    }

	    if (!strcmp(THP_MODEL, "BMP280")) {
	      if (checkBmpStatus() == true) {
		      message.replace("{MQTT_TEMP}", String(int(currentTemperature)));
		  	  message.replace("{MQTT_TOPIC_TEMP}", _addMQTTTextInput("MQTT_TOPIC_TEMP", MQTT_TOPIC_TEMP));
	
		      message.replace("<b>{TEXT_HUMI_TOPIC}: </b>/{MQTT_IP}{MQTT_DEVICENAME}{MQTT_TOPIC_HUMI}/{MQTT_HUMI}<br />", "");
	
		      message.replace("{MQTT_PRESS}", String(int(currentPressure)));
		      message.replace("{MQTT_TOPIC_PRESS}", _addMQTTTextInput("MQTT_TOPIC_PRESS", MQTT_TOPIC_PRESS));
	      } else {
	        if (DEBUG) {
	          Serial.println("No measurements from BMP280!\n");
	        }
	      }
	    }

	    if (!strcmp(THP_MODEL, "HTU21")) {
	      if (checkHTU21DStatus() == true) {
		      message.replace("{MQTT_TEMP}", String(int(currentTemperature)));
		  	  message.replace("{MQTT_TOPIC_TEMP}", _addMQTTTextInput("MQTT_TOPIC_TEMP", MQTT_TOPIC_TEMP));
	
		      message.replace("{MQTT_HUMI}", String(int(currentHumidity)));
		      message.replace("{MQTT_TOPIC_HUMI}", _addMQTTTextInput("MQTT_TOPIC_HUMI", MQTT_TOPIC_HUMI));
	
		      message.replace("<b>{TEXT_PRESS_TOPIC}: </b>/{MQTT_IP}{MQTT_DEVICENAME}{MQTT_TOPIC_PRESS}/{MQTT_PRESS}<br />", "");
	      } else {
	        if (DEBUG) {
	          Serial.println("No measurements from HTU21!\n");
	        }
	      }
	    }

	    if (!strcmp(THP_MODEL, "DHT22")) {
	      if (checkDHT22Status() == true) {
		      message.replace("{MQTT_TEMP}", String(int(currentTemperature)));
		  	  message.replace("{MQTT_TOPIC_TEMP}", _addMQTTTextInput("MQTT_TOPIC_TEMP", MQTT_TOPIC_TEMP));
	
		      message.replace("{MQTT_HUMI}", String(int(currentHumidity)));
		      message.replace("{MQTT_TOPIC_HUMI}", _addMQTTTextInput("MQTT_TOPIC_HUMI", MQTT_TOPIC_HUMI));
	
		      message.replace("<b>PRESS: </b>/{MQTT_IP}{MQTT_DEVICENAME}{MQTT_TOPIC_PRESS}/{MQTT_PRESS}<br />", "");
	      } else {
	        if (DEBUG) {
	          Serial.println("No measurements from DHT22!\n");
	        }
	      }
	    }

	    if (!strcmp(THP_MODEL, "SHT1x")) {
	      if (checkDHT22Status() == true) {
		      message.replace("{MQTT_TEMP}", String(int(currentTemperature)));
		  	  message.replace("{MQTT_TOPIC_TEMP}", _addMQTTTextInput("MQTT_TOPIC_TEMP", MQTT_TOPIC_TEMP));
	
		      message.replace("{MQTT_HUMI}", String(int(currentHumidity)));
		      message.replace("{MQTT_TOPIC_HUMI}", _addMQTTTextInput("MQTT_TOPIC_HUMI", MQTT_TOPIC_HUMI));
	
		      message.replace("<b>{TEXT_PRESS_TOPIC}: </b>/{MQTT_IP}{MQTT_DEVICENAME}{MQTT_TOPIC_PRESS}/{MQTT_PRESS}<br />", "");
	      } else {
	        if (DEBUG) {
	          Serial.println("No measurements from SHT1x!\n");
	        }
	      }
	    }	
	  } else {
		  message.replace("<b>{TEXT_TEMP_TOPIC}: </b>/{MQTT_IP}{MQTT_DEVICENAME}{MQTT_TOPIC_TEMP}/{MQTT_TEMP}<br />", "");
		  message.replace("<b>{TEXT_HUMI_TOPIC}: </b>/{MQTT_IP}{MQTT_DEVICENAME}{MQTT_TOPIC_HUMI}/{MQTT_HUMI}<br />", "");
		  message.replace("<b>{TEXT_PRESS_TOPIC}: </b>/{MQTT_IP}{MQTT_DEVICENAME}{MQTT_TOPIC_PRESS}/{MQTT_PRESS}<br />", "");
	  }

	 if (strcmp(DUST_MODEL, "Non")) {
	     message.replace("{MQTT_PM1}", String(int(averagePM1)));
		 message.replace("{MQTT_TOPIC_PM1}", _addMQTTTextInput("MQTT_TOPIC_PM1", MQTT_TOPIC_PM1));
	     message.replace("{MQTT_PM25}", String(int(averagePM25)));
		 message.replace("{MQTT_TOPIC_PM25}", _addMQTTTextInput("MQTT_TOPIC_PM25", MQTT_TOPIC_PM25));
	     message.replace("{MQTT_PM10}", String(int(averagePM10)));
		 message.replace("{MQTT_TOPIC_PM10}", _addMQTTTextInput("MQTT_TOPIC_PM10", MQTT_TOPIC_PM10));
	  
	     if (averagePM25 <= 10) {
	   	  message.replace("{MQTT_AIRQUALITY}", "EXCELLENT");
	     } else if (averagePM25 > 10 && averagePM25 <= 20) {
	   	  message.replace("{MQTT_AIRQUALITY}", "GOOD");
	     } else if (averagePM25 > 20 && averagePM25 <= 25) {
	   	  message.replace("{MQTT_AIRQUALITY}", "FAIR");
	     } else if (averagePM25 > 25 && averagePM25 <= 50) {
	   	  message.replace("{MQTT_AIRQUALITY}", "INFERIOR");
	     } else if (averagePM25 > 50) {
	   	  message.replace("{MQTT_AIRQUALITY}", "POOR");
	     } else {
	   	  message.replace("{MQTT_AIRQUALITY}", "UNKNOWN");
	     }
	     message.replace("{MQTT_TOPIC_AIRQUALITY}", _addMQTTTextInput("MQTT_TOPIC_AIRQUALITY", MQTT_TOPIC_AIRQUALITY));
	 } else {
	  message.replace("<b>{TEXT_PM1_TOPIC}: </b>/{MQTT_IP}{MQTT_DEVICENAME}{MQTT_TOPIC_PM1}/{MQTT_PM1}<br />", "");
	  message.replace("<b>{TEXT_PM25_TOPIC}: </b>/{MQTT_IP}{MQTT_DEVICENAME}{MQTT_TOPIC_PM25}/{MQTT_PM25}<br />", "");
	  message.replace("<b>{TEXT_PM10_TOPIC}: </b>/{MQTT_IP}{MQTT_DEVICENAME}{MQTT_TOPIC_PM10}/{MQTT_PM10}<br />", "");
	  message.replace("<b>{TEXT_AIRQUALITY_TOPIC}: </b>/{MQTT_IP}{MQTT_DEVICENAME}{MQTT_TOPIC_AIRQUALITY}/{MQTT_AIRQUALITY}<br />", "");
	 }
 
	    message.replace("{TEXT_TEMP_TOPIC}", (TEXT_TEMPERATURE));
	    message.replace("{TEXT_HUMI_TOPIC}", (TEXT_HUMIDITY));
	    message.replace("{TEXT_PRESS_TOPIC}", (TEXT_PRESSURE));
	    message.replace("{TEXT_PM1_TOPIC}", "PM1");
		message.replace("{TEXT_PM25_TOPIC}", "PM2.5");
		message.replace("{TEXT_PM10_TOPIC}", "PM10");
		message.replace("{TEXT_AIRQUALITY_TOPIC}", (TEXT_AIRQUALITY_TOPIC));

	  if (MQTT_DEVICENAME_IN_TOPIC) {
		  message.replace("{MQTT_DEVICENAME}", (String(device_name) + "/"));
	  } else {
		  message.replace("{MQTT_DEVICENAME}", "");
	  }
	  if (MQTT_IP_IN_TOPIC) {
			  message.replace("/{MQTT_IP}", ("/" + String(WiFi.localIP().toString()) + "/"));	  	
	  } else {
		  message.replace("/{MQTT_IP}", "/");
	  }
	  
	  message.replace("<br><center>{AdvancedMQTTConfigButton}</center><br>", "");
	  
	  message.replace("{WiFiEraseButton}", _addWiFiErase());
	  message.replace("{RestoreConfigButton}", _addRestoreConfig());
	  message.replace("{SubmitButton}", _addSubmitServices());
	  message += FPSTR(WEB_PAGE_FOOTER);
#endif

  //WebServer.send(200, "text/html", message);
  request->send(200, "text/html", message);
}

bool _parseAsBool(String value) {
  return value == "yes";
}

void _set_language() {
  if (!strcmp(LANGUAGE, "english")) {
    SELECTED_LANGUAGE = 1;
  } else if (!strcmp(LANGUAGE, "polish")) {
    SELECTED_LANGUAGE = 2;
  } else {
    SELECTED_LANGUAGE = 1;
  }
}

void _parseAsCString(char* dest, String value) {
  strncpy(dest, value.c_str(), 255);
}

/*
void handle_config_device() {
  _handle_config_device(false);
}

void handle_config_services() {
  _handle_config_services(false);
}
*/

//void handle_config_device_post() {
void handle_config_device_save(AsyncWebServerRequest *request) {
  int need_update = 0;
  /*
  if (DEBUG) {
    Serial.println("POST CONFIG START!!");
    int argsLen = WebServer.args();
    for (int i = 0; i < argsLen; i++) {
      String argName = WebServer.argName(i);
      String arg = WebServer.arg(i);
      String ss = "** ";
      ss += argName;
      ss += " = ";
      ss += arg;
      Serial.println(ss);
    }
  }

  // REMEMBER TO ADD/EDIT KEYS IN config.h AND spiffs.cpp!!
  */

	if (request->hasParam("DEVICENAME_AUTO")) {
		DEVICENAME_AUTO = _parseAsBool(request->getParam("DEVICENAME_AUTO")->value());
	}
	
if (!DEVICENAME_AUTO) {
	if (request->hasParam("DEVICENAME")) {
		_parseAsCString(DEVICENAME, request->getParam("DEVICENAME")->value());
	}
}	
	if (request->hasParam("DISPLAY_PM1")) {
		DISPLAY_PM1 = _parseAsBool(request->getParam("DISPLAY_PM1")->value());
	}
	
	if (request->hasParam("LANGUAGE")) {
    	_parseAsCString(LANGUAGE, request->getParam("LANGUAGE")->value());
    	_set_language();
	}
	
    char oldTHP_MODEL[32];
    strcpy(oldTHP_MODEL, THP_MODEL);
	if (request->hasParam("THP_MODEL")) {
		_parseAsCString(THP_MODEL, request->getParam("THP_MODEL")->value());
	}
    if (strcmp(THP_MODEL, oldTHP_MODEL) and !strcmp(THP_MODEL, "BME280-SparkFun")) {
      need_update = 1;
    }
	
    char oldDUST_MODEL[32];
    strcpy(oldDUST_MODEL, DUST_MODEL);
	if (request->hasParam("DUST_MODEL")) {
		_parseAsCString(DUST_MODEL, request->getParam("DUST_MODEL")->value());
	}
	
    // DUST Sensor config - START
    if (!strcmp(PMSENSORVERSION, "PMS")) {
      if (strcmp(DUST_MODEL, oldDUST_MODEL) and !strcmp(DUST_MODEL, "SDS011/21")) {
        need_update = 2;
      }
      if (strcmp(DUST_MODEL, oldDUST_MODEL) and !strcmp(DUST_MODEL, "HPMA115S0")) {
        need_update = 3;
      }
      if (strcmp(DUST_MODEL, oldDUST_MODEL) and !strcmp(DUST_MODEL, "SPS30")) {
        need_update = 5;
      }
    } else if (!strcmp(PMSENSORVERSION, "SDS")) {
      if (strcmp(DUST_MODEL, oldDUST_MODEL) and !strcmp(DUST_MODEL, "HPMA115S0")) {
        need_update = 3;
      }
      if (strcmp(DUST_MODEL, oldDUST_MODEL) and !strcmp(DUST_MODEL, "PMS7003")) {
        need_update = 4;
      }
      if (strcmp(DUST_MODEL, oldDUST_MODEL) and !strcmp(DUST_MODEL, "SPS30")) {
        need_update = 5;
      }
    } else if (!strcmp(PMSENSORVERSION, "HPMA115S0")) {
      if (strcmp(DUST_MODEL, oldDUST_MODEL) and !strcmp(DUST_MODEL, "SDS011/21")) {
        need_update = 2;
      }
      if (strcmp(DUST_MODEL, oldDUST_MODEL) and !strcmp(DUST_MODEL, "PMS7003")) {
        need_update = 4;
      }
      if (strcmp(DUST_MODEL, oldDUST_MODEL) and !strcmp(DUST_MODEL, "SPS30")) {
        need_update = 5;
      }
    } else if (!strcmp(PMSENSORVERSION, "SPS30")) {
      if (strcmp(DUST_MODEL, oldDUST_MODEL) and !strcmp(DUST_MODEL, "SDS011/21")) {
        need_update = 2;
      }
      if (strcmp(DUST_MODEL, oldDUST_MODEL) and !strcmp(DUST_MODEL, "HPMA115S0")) {
        need_update = 3;
      }
      if (strcmp(DUST_MODEL, oldDUST_MODEL) and !strcmp(DUST_MODEL, "PMS7003")) {
        need_update = 4;
      }
    } else if (!strcmp(PMSENSORVERSION, "PMS-SparkFunBME280")) {
      if (strcmp(DUST_MODEL, oldDUST_MODEL) and !strcmp(DUST_MODEL, "SDS011/21")) {
        need_update = 2;
      }
      if (strcmp(DUST_MODEL, oldDUST_MODEL) and !strcmp(DUST_MODEL, "HPMA115S0")) {
        need_update = 3;
      }
      if (strcmp(DUST_MODEL, oldDUST_MODEL) and !strcmp(DUST_MODEL, "SPS30")) {
        need_update = 5;
      }
    }
    // DUST Sensor config - END
	
	if (request->hasParam("FREQUENTMEASUREMENT")) {
		FREQUENTMEASUREMENT = _parseAsBool(request->getParam("FREQUENTMEASUREMENT")->value());
	}
	
	if (request->hasParam("DUST_TIME")) {
		DUST_TIME = (request->getParam("DUST_TIME")->value()).toInt();
	}
	
	if (request->hasParam("NUMBEROFMEASUREMENTS")) {
		NUMBEROFMEASUREMENTS = (request->getParam("NUMBEROFMEASUREMENTS")->value()).toInt();
	}
	
	if (request->hasParam("DEEPSLEEP_ON")) {
		DEEPSLEEP_ON = _parseAsBool(request->getParam("DEEPSLEEP_ON")->value());
	}
	
	if (request->hasParam("MYALTITUDE")) {
		MYALTITUDE = (request->getParam("MYALTITUDE")->value()).toInt();
	}
	
	if (request->hasParam("CONFIG_AUTH")) {
		CONFIG_AUTH = _parseAsBool(request->getParam("CONFIG_AUTH")->value());
	}
	
	if (request->hasParam("CONFIG_USERNAME")) {
		_parseAsCString(CONFIG_USERNAME, request->getParam("CONFIG_USERNAME")->value());
	}
	
	if (request->hasParam("CONFIG_PASSWORD")) {
		_parseAsCString(CONFIG_PASSWORD, request->getParam("CONFIG_PASSWORD")->value());
	}
	
	if (request->hasParam("DEBUG")) {
		DEBUG = _parseAsBool(request->getParam("DEBUG")->value());
	}
	
	if (request->hasParam("MODEL")) {
		_parseAsCString(MODEL, request->getParam("MODEL")->value());
	}
	
	if (request->hasParam("AUTOUPDATE_ON")) {
		AUTOUPDATE_ON = _parseAsBool(request->getParam("AUTOUPDATE_ON")->value());
	}

  if (DEBUG) {
    Serial.println("POST DEVICE CONFIG END!!");
  }

  if (need_update != 0) {
    strcpy(THP_MODEL, "Non");
    strcpy(DUST_MODEL, "Non");
    saveConfig();
    //_handle_config_device(true);
    if (need_update == 1) {
      doUpdate(1); // BME280-SparkFun
    }
    if (need_update == 2) {
      doUpdate(2); // SDS011
    }
    if (need_update == 3) {
      doUpdate(3); // HPMA115S0
    }
    if (need_update == 4) {
      doUpdate(4); // PMSx003
    }
    if (need_update >= 5) {
      doUpdate(5); // SPS30
    }
    if (need_update >= 6) {
      doUpdate(0); // CURRENT SERVERSOFTWARE VERSION
    }
  }

  saveConfig();
  //delay(250);
  //_handle_config_device(true);
  // https://github.com/esp8266/Arduino/issues/1722
  //ESP.reset();
  delay(300);
  request->redirect("/");
    /*
  int redirect_interval = 3 * 1000; // 3 seconds
  unsigned long current_redirect_Millis = millis();
  unsigned long previous_redirect_Millis = millis();
  while (previous_redirect_Millis - current_redirect_Millis <= redirect_interval * 1) {
	  previous_redirect_Millis = millis();
  }
  */
  delay(1000);
  ESP.restart();
}

//void handle_config_services_post() {
void handle_config_services_save(AsyncWebServerRequest *request) {
	/*
  if (DEBUG) {
    Serial.println("POST CONFIG START!!");
    int argsLen = WebServer.args();
    for (int i = 0; i < argsLen; i++) {
      String argName = WebServer.argName(i);
      String arg = WebServer.arg(i);
      String ss = "** ";
      ss += argName;
      ss += " = ";
      ss += arg;
      Serial.println(ss);
    }
  }

  // REMEMBER TO ADD/EDIT KEYS IN config.h AND spiffs.cpp!!
  */
	if (request->hasParam("SENDING_FREQUENCY")) {
    	SENDING_FREQUENCY = (request->getParam("SENDING_FREQUENCY")->value()).toInt();
	}

	if (request->hasParam("SENDING_DB_FREQUENCY")) {
		SENDING_DB_FREQUENCY = (request->getParam("SENDING_DB_FREQUENCY")->value()).toInt();
	}

	if (request->hasParam("SMOGLIST_ON")) {
		SMOGLIST_ON = _parseAsBool(request->getParam("SMOGLIST_ON")->value());
	}
	
	if (request->hasParam("LUFTDATEN_ON")) {
		LUFTDATEN_ON = _parseAsBool(request->getParam("LUFTDATEN_ON")->value());
	}

	if (request->hasParam("AQI_ECO_ON")) {
		AQI_ECO_ON = _parseAsBool(request->getParam("AQI_ECO_ON")->value());
	}

	if (request->hasParam("AQI_ECO_HOST")) {
		_parseAsCString(AQI_ECO_HOST, request->getParam("AQI_ECO_HOST")->value());
	}
	
	if (request->hasParam("AQI_ECO_PATH")) {
		_parseAsCString(AQI_ECO_PATH, request->getParam("AQI_ECO_PATH")->value());
	}
	
	if (request->hasParam("AIRMONITOR_ON")) {
		AIRMONITOR_ON = _parseAsBool(request->getParam("AIRMONITOR_ON")->value());
	}

	if (request->hasParam("AIRMONITOR_GRAPH_ON")) {
		AIRMONITOR_GRAPH_ON = _parseAsBool(request->getParam("AIRMONITOR_GRAPH_ON")->value());
	}
	
	if (request->hasParam("LATITUDE")) {
		_parseAsCString(LATITUDE, request->getParam("LATITUDE")->value());
	}
	
	if (request->hasParam("LONGITUDE")) {
		_parseAsCString(LONGITUDE, request->getParam("LONGITUDE")->value());
	}
	
	if (request->hasParam("THINGSPEAK_ON")) {
		THINGSPEAK_ON = _parseAsBool(request->getParam("THINGSPEAK_ON")->value());
	}

	if (request->hasParam("THINGSPEAK_GRAPH_ON")) {
		THINGSPEAK_GRAPH_ON = _parseAsBool(request->getParam("THINGSPEAK_GRAPH_ON")->value());
	}
	
	if (request->hasParam("THINGSPEAK_API_KEY")) {
		_parseAsCString(THINGSPEAK_API_KEY, request->getParam("THINGSPEAK_API_KEY")->value());
	}
	
	if (request->hasParam("THINGSPEAK_CHANNEL_ID")) {
		THINGSPEAK_CHANNEL_ID = (request->getParam("THINGSPEAK_CHANNEL_ID")->value()).toInt();
	}
	
	if (request->hasParam("THINGSPEAK_READ_API_KEY")) {
		_parseAsCString(THINGSPEAK_READ_API_KEY, request->getParam("THINGSPEAK_READ_API_KEY")->value());
	}
	
	if (request->hasParam("INFLUXDB_ON")) {
		INFLUXDB_ON = _parseAsBool(request->getParam("INFLUXDB_ON")->value());
	}
	
	if (request->hasParam("INFLUXDB_VERSION")) {
		_parseAsCString(INFLUXDB_VERSION, request->getParam("INFLUXDB_VERSION")->value());
	}	
	
	if (request->hasParam("INFLUXDB_HOST")) {
		_parseAsCString(INFLUXDB_HOST, request->getParam("INFLUXDB_HOST")->value());
	}
	
	if (request->hasParam("INFLUXDB_PORT")) {
		INFLUXDB_PORT = (request->getParam("INFLUXDB_PORT")->value()).toInt();
	}
	
	if (request->hasParam("INFLUXDB_DATABASE")) {
		_parseAsCString(INFLUXDB_DATABASE, request->getParam("INFLUXDB_DATABASE")->value());
	}
	
	if (request->hasParam("INFLUXDB_USER")) {
		_parseAsCString(INFLUXDB_USER, request->getParam("INFLUXDB_USER")->value());
	}
	
	if (request->hasParam("INFLUXDB_PASSWORD")) {
		_parseAsCString(INFLUXDB_PASSWORD, request->getParam("INFLUXDB_PASSWORD")->value());
	}

	if (request->hasParam("INFLUXDB_ORG")) {
		_parseAsCString(INFLUXDB_ORG, request->getParam("INFLUXDB_ORG")->value());
	}
	if (request->hasParam("INFLUXDB_BUCKET")) {
		_parseAsCString(INFLUXDB_BUCKET, request->getParam("INFLUXDB_BUCKET")->value());
	}
	if (request->hasParam("INFLUXDB_TOKEN")) {
		_parseAsCString(INFLUXDB_TOKEN, request->getParam("INFLUXDB_TOKEN")->value());
	}

	if (request->hasParam("MQTT_ON")) {
		MQTT_ON = _parseAsBool(request->getParam("MQTT_ON")->value());
	}
	
	if (request->hasParam("MQTT_HOST")) {
		_parseAsCString(MQTT_HOST, request->getParam("MQTT_HOST")->value());
	}
	
	if (request->hasParam("MQTT_PORT")) {
		MQTT_PORT = (request->getParam("MQTT_PORT")->value()).toInt();
	}
	
	if (request->hasParam("MQTT_USER")) {
		_parseAsCString(MQTT_USER, request->getParam("MQTT_USER")->value());
	}
	
	if (request->hasParam("MQTT_PASSWORD")) {
		_parseAsCString(MQTT_PASSWORD, request->getParam("MQTT_PASSWORD")->value());
	}
	
	if (request->hasParam("MQTT_IP_IN_TOPIC")) {
		MQTT_IP_IN_TOPIC = _parseAsBool(request->getParam("MQTT_IP_IN_TOPIC")->value());
	}
	
	if (request->hasParam("MQTT_DEVICENAME_IN_TOPIC")) {
		MQTT_DEVICENAME_IN_TOPIC = _parseAsBool(request->getParam("MQTT_DEVICENAME_IN_TOPIC")->value());
	}

	if (request->hasParam("MQTT_TOPIC_TEMP")) {
		_parseAsCString(MQTT_TOPIC_TEMP, request->getParam("MQTT_TOPIC_TEMP")->value());
	}
	
	if (request->hasParam("MQTT_TOPIC_HUMI")) {
		_parseAsCString(MQTT_TOPIC_HUMI, request->getParam("MQTT_TOPIC_HUMI")->value());
	}
	
	if (request->hasParam("MQTT_TOPIC_PRESS")) {
		_parseAsCString(MQTT_TOPIC_PRESS, request->getParam("MQTT_TOPIC_PRESS")->value());
	}
	
	if (request->hasParam("MQTT_TOPIC_PM1")) {
		_parseAsCString(MQTT_TOPIC_PM1, request->getParam("MQTT_TOPIC_PM1")->value());
	}
	
	if (request->hasParam("MQTT_TOPIC_PM25")) {
		_parseAsCString(MQTT_TOPIC_PM25, request->getParam("MQTT_TOPIC_PM25")->value());
	}
	
	if (request->hasParam("MQTT_TOPIC_PM10")) {
		_parseAsCString(MQTT_TOPIC_PM10, request->getParam("MQTT_TOPIC_PM10")->value());
	}
	
	if (request->hasParam("MQTT_TOPIC_AIRQUALITY")) {
		_parseAsCString(MQTT_TOPIC_AIRQUALITY, request->getParam("MQTT_TOPIC_AIRQUALITY")->value());
	}

  if (DEBUG) {
    Serial.println("POST SERVICES CONFIG END!!");
  }
  
  saveConfig();
  //delay(250);
  //_handle_config_services(true);
  // https://github.com/esp8266/Arduino/issues/1722
  //ESP.reset();
  delay(300);
  request->redirect("/");
  delay(1000);
  ESP.restart();
}
  
static void handle_update_progress_cb(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final) {
  uint32_t free_space = (ESP.getFreeSketchSpace() - 0x1000) & 0xFFFFF000;
  if (!index){
    Serial.println("Update");
    //Update.runAsync(true);
    if (!Update.begin(free_space)) {
      Update.printError(Serial);
    }
  }

  if (Update.write(data, len) != len) {
    Update.printError(Serial);
  }

  if (final) {
    if (!Update.end(true)){
      Update.printError(Serial);
    } else {
      Serial.println("Update complete");
	  Serial.println("Restart");
	  delay(1000);
	  ESP.restart();
    }
  }
}

void handle_update_done(AsyncWebServerRequest *request) {
	request->send(200);
	}

//void handle_update() {            //Handler for the handle_update
void handle_update(AsyncWebServerRequest *request) {
    if (CONFIG_AUTH == true) {
  	  /*
      if (!server.authenticate(CONFIG_USERNAME, CONFIG_PASSWORD)) {
        //return server.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
        return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
      }
  	*/
  	  if(!request->authenticate(CONFIG_USERNAME,CONFIG_PASSWORD,NULL,false)) {
  	        request->requestAuthentication(NULL,false); // force basic auth
  	      }
    }
    String message = FPSTR(WEB_PAGE_HEADER);
  message.replace("{WEB_PAGE_CSS}", FPSTR(WEB_PAGE_HEADER_CSS));
  message.replace("{Language}", (TEXT_LANG));
  message.replace("{CurrentPageTitle}", (TEXT_UPDATE_PAGE));
  message.replace("{IndexPageTitle}", (TEXT_INDEX_PAGE));
  message.replace("{ConfigPageTitle}", (TEXT_CONFIG_PAGE));
  message.replace("{UpdatePageTitle}", (TEXT_UPDATE_PAGE));

  message += FPSTR(WEB_UPDATE_PAGE_UPDATE);

  if (!AUTOUPDATE_ON) {
    if (need_update) {
      message.replace("{WEB_UPDATE_INFO_WARNING}", FPSTR(WEB_UPDATE_INFO_WARNING));
      message.replace("{TEXT_FWUPDATEAVALIBLE}", (TEXT_FWUPDATEAVALIBLE));
      message.replace("{MANUALUPDATEBUTTON}", "");
      message.replace("{FWUPDATEBUTTON}", FPSTR(WEB_UPDATE_BUTTON_FWUPDATE));
      message.replace("{TEXT_FWUPDATEBUTTON}", (TEXT_FWUPDATEBUTTON));
      message.replace("{AUTOUPDATEONBUTTON}", FPSTR(WEB_UPDATE_BUTTON_AUTOUPDATEON));
      message.replace("{TEXT_AUTOUPDATEONBUTTON}", (TEXT_AUTOUPDATEONBUTTON));
      message.replace("{TEXT_AUTOUPDATEWARNING}", (TEXT_AUTOUPDATEWARNING));
      message.replace("{TEXT_FWUPDATEBUTTON}", (TEXT_FWUPDATEBUTTON));
    }
    message.replace("{WEB_UPDATE_INFO_WARNING}", "");
  } else {
    message.replace("{WEB_UPDATE_INFO_WARNING}", "");
  }

  message.replace("{TEXT_UPDATE_PAGE}", (TEXT_UPDATE_PAGE));
  message.replace("{TEXT_SELECTUPDATEFILE}", (TEXT_SELECTUPDATEFILE));
  message.replace("{TEXT_SUBMITUPDATE}", (TEXT_SUBMITUPDATE));

  message.replace("{TEXT_AUTOUPDATEON}", (TEXT_AUTOUPDATEON));
  if (AUTOUPDATE_ON) {
    message.replace("{AUTOUPDATEONSTATUS}", (TEXT_YES));
  } else {
    message.replace("{AUTOUPDATEONSTATUS}", (TEXT_NO));
  }

  message.replace("{TEXT_CURRENTSOFTVERSION}", (TEXT_CURRENTSOFTVERSION));
  message.replace("{SOFTWAREVERSION}", String(CURRENTSOFTWAREVERSION) + " " + String(PMSENSORVERSION));

  message.replace("{TEXT_SERVERSOFTWAREVERSION}", (TEXT_SERVERSOFTWAREVERSION));
  message.replace("{SERVERSOFTWAREVERSION}", String(SERVERSOFTWAREVERSION) + " " + String(PMSENSORVERSION));

  message.replace("{TEXT_LATESTAVAILABLESOFT}", TEXT_LATESTAVAILABLESOFT);
  message.replace("{SMOGOMIERZRELEASES_LINK}", (SMOGOMIERZRELEASES_LINK));
  message.replace("{TEXT_HERE}", (TEXT_HERE));

  // init WiFi signal quality info - START
  String WiFiSSID = WiFi.SSID();
  int WiFiRSSI = WiFi.RSSI();
  message.replace("{TEXT_CONNECTEDWIFI}", (TEXT_CONNECTEDWIFI));
  message.replace("{WiFiSSID}", (WiFiSSID));
  message.replace("{TEXT_WIFIRSSI}", (TEXT_WIFIRSSI));
  message.replace("{WiFiRSSI}", (String(WiFiRSSI) + " dBm"));

  // https://stackoverflow.com/a/15798024
  int WiFiQuality;
  if (WiFiRSSI <= -100) {
    WiFiQuality = 0;
  } else if (WiFiRSSI >= -50) {
    WiFiQuality = 100;
  } else {
    WiFiQuality = 2 * (WiFiRSSI + 100);
  }
  message.replace("{TEXT_WIFIQUALITY}", (TEXT_WIFIQUALITY));
  message.replace("{WiFiQuality}", (String(WiFiQuality) + " %"));
  // init WiFi signal quality info - END
  
  message += FPSTR(WEB_PAGE_FOOTER);
  //WebServer.send(200, "text/html", message);
  request->send(200, "text/html", message);
}

//void erase_wifi() {
void erase_wifi(AsyncWebServerRequest *request) {
    if (CONFIG_AUTH == true) {
  	  /*
      if (!server.authenticate(CONFIG_USERNAME, CONFIG_PASSWORD)) {
        //return server.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
        return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
      }
  	*/
  	  if(!request->authenticate(CONFIG_USERNAME,CONFIG_PASSWORD,NULL,false)) {
  	        request->requestAuthentication(NULL,false); // force basic auth
  	      }
    }
  Serial.println("Erasing Config...");
#ifdef ARDUINO_ARCH_ESP8266
  ESP.eraseConfig();
#elif defined ARDUINO_ARCH_ESP32
  WiFi.disconnect(false, true);
#endif
  /*
  WebServer.sendHeader("Location", "/", true);
  WebServer.send ( 302, "text/plain", "");
  */
  request->redirect("/");
  delay(1000);
  Serial.println("Restart");
  ESP.restart();
}

//void restore_config() {
void restore_config(AsyncWebServerRequest *request) {
    if (CONFIG_AUTH == true) {
  	  /*
      if (!server.authenticate(CONFIG_USERNAME, CONFIG_PASSWORD)) {
        //return server.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
        return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
      }
  	*/
  	  if(!request->authenticate(CONFIG_USERNAME,CONFIG_PASSWORD,NULL,false)) {
  	        request->requestAuthentication(NULL,false); // force basic auth
  	      }
    }
  Serial.println("Restoring default settings...");
  deleteConfig();
  /*
  WebServer.sendHeader("Location", "/", true);
  WebServer.send ( 302, "text/plain", "");
  */
  request->redirect("/");
  delay(1000);
  Serial.println("Restart");
  ESP.restart();
}

//void fwupdate() {
void fwupdate(AsyncWebServerRequest *request) {
    if (CONFIG_AUTH == true) {
  	  /*
      if (!server.authenticate(CONFIG_USERNAME, CONFIG_PASSWORD)) {
        //return server.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
        return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
      }
  	*/
  	  if(!request->authenticate(CONFIG_USERNAME,CONFIG_PASSWORD,NULL,false)) {
  	        request->requestAuthentication(NULL,false); // force basic auth
  	      }
    }
  doUpdate(0);
  delay(1000);
  /*
  WebServer.sendHeader("Location", "/", true);
  WebServer.send ( 302, "text/plain", "");
  */
  request->redirect("/");
  delay(1000);
}

//void autoupdate_on() {
void autoupdate_on(AsyncWebServerRequest *request) {
    if (CONFIG_AUTH == true) {
  	  /*
      if (!server.authenticate(CONFIG_USERNAME, CONFIG_PASSWORD)) {
        //return server.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
        return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
      }
  	*/
  	  if(!request->authenticate(CONFIG_USERNAME,CONFIG_PASSWORD,NULL,false)) {
  	        request->requestAuthentication(NULL,false); // force basic auth
  	      }
    }
  AUTOUPDATE_ON = true;
  saveConfig();
  /*
  WebServer.sendHeader("Location", "/", true);
  WebServer.send ( 302, "text/plain", "");
  */
  request->redirect("/");
  delay(1000);
  Serial.println("Restart");
  ESP.restart();
}

//void handle_api() {
void handle_api(AsyncWebServerRequest *request) {
  String message;
  StaticJsonDocument<800> jsonBuffer;
  JsonObject json = jsonBuffer.to<JsonObject>();

  json["device_name"] = device_name;
  if (strcmp(DUST_MODEL, "Non")) {
    json["pm1"] = averagePM1;
    json["pm25"] = averagePM25;
	if (!strcmp(DUST_MODEL, "SPS30")) {
	json["pm4"] = averagePM4;
	}
    json["pm10"] = averagePM10;
  }
  if (strcmp(THP_MODEL, "Non")) {
    takeTHPMeasurements();
  }
  if (!strcmp(THP_MODEL, "BME280")) {
    if (checkBmeStatus()) {
      json["temperature"] = float(currentTemperature);
      json["pressure"] = int(currentPressure);
      json["humidity"] = int(currentHumidity);
      json["dewpoint"] = float(pow((currentHumidity) / 100, 0.125) * (112 + 0.9 * (currentTemperature)) + 0.1 * (currentTemperature) - 112);
    }
  }
  if (!strcmp(THP_MODEL, "BMP280")) {
    if (checkBmpStatus()) {
      json["temperature"] = float(currentTemperature);
      json["pressure"] = int(currentPressure);
    }
  }
  if (!strcmp(THP_MODEL, "HTU21")) {
    if (checkHTU21DStatus()) {
      json["temperature"] = float(currentTemperature);
      json["humidity"] = int(currentHumidity);
      json["dewpoint"] = float(pow((currentHumidity) / 100, 0.125) * (112 + 0.9 * (currentTemperature)) + 0.1 * (currentTemperature) - 112);
    }
  }
  if (!strcmp(THP_MODEL, "DHT22")) {
    if (checkDHT22Status()) {
      json["temperature"] = float(currentTemperature);
      json["humidity"] = int(currentHumidity);
      json["dewpoint"] = float(pow((currentHumidity) / 100, 0.125) * (112 + 0.9 * (currentTemperature)) + 0.1 * (currentTemperature) - 112);
    }
  }
  if (!strcmp(THP_MODEL, "SHT1x")) {
    if (checkSHT1xStatus()) {
      json["temperature"] = float(currentTemperature);
      json["humidity"] = int(currentHumidity);
      json["dewpoint"] = float(pow((currentHumidity) / 100, 0.125) * (112 + 0.9 * (currentTemperature)) + 0.1 * (currentTemperature) - 112);
    }
  }

  serializeJsonPretty(json, message);
  //WebServer.send(200, "text/json", message);
  request->send(200, "text/json", message);
}

void homekit_reset(AsyncWebServerRequest *request) {
  if (CONFIG_AUTH == true) {
    /*
      if (!server.authenticate(CONFIG_USERNAME, CONFIG_PASSWORD)) {
      //return server.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
    */
    if (!request->authenticate(CONFIG_USERNAME, CONFIG_PASSWORD, NULL, false)) {
      request->requestAuthentication(NULL, false); // force basic auth
    }
  }
  Serial.println("reset homekit...");

  String pair_file_name = "/homekit_pair.dat";
  SPIFFS.remove(pair_file_name);
  
  request->redirect("/");

  //request->redirect("/");
  delay(1000);
  //Serial.println("Restart");
  //ESP.restart();
}

void homekit_on(AsyncWebServerRequest *request) {
  if (CONFIG_AUTH == true) {
    /*
      if (!server.authenticate(CONFIG_USERNAME, CONFIG_PASSWORD)) {
      //return server.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
    */
    if (!request->authenticate(CONFIG_USERNAME, CONFIG_PASSWORD, NULL, false)) {
      request->requestAuthentication(NULL, false); // force basic auth
    }
  }
  Serial.println("homekit on...");

  HOMEKIT_SUPPORT = true;
  saveConfig();
  
  request->redirect("/");

  delay(1000);
  Serial.println("Restart");
  ESP.restart();
}

void homekit_off(AsyncWebServerRequest *request) {
  if (CONFIG_AUTH == true) {
    /*
      if (!server.authenticate(CONFIG_USERNAME, CONFIG_PASSWORD)) {
      //return server.requestAuthentication(BASIC_AUTH, www_realm, authFailResponse);
      return server.requestAuthentication(DIGEST_AUTH, www_realm, authFailResponse);
    */
    if (!request->authenticate(CONFIG_USERNAME, CONFIG_PASSWORD, NULL, false)) {
      request->requestAuthentication(NULL, false); // force basic auth
    }
  }
  Serial.println("homekit off...");

  HOMEKIT_SUPPORT = false;
  saveConfig();
  
  request->redirect("/");

  delay(1000);
  Serial.println("Restart");
  ESP.restart();
}