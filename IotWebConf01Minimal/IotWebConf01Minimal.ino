/**
 * IotWebConf01Minimal.ino -- IotWebConf is an ESP8266/ESP32
 *   non blocking WiFi/AP web configuration library for Arduino.
 *   https://github.com/prampec/IotWebConf 
 *
 * Copyright (C) 2018 Balazs Kelemen <prampec+arduino@gmail.com>
 *
 * This software may be modified and distributed under the terms
 * of the MIT license.  See the LICENSE file for details.
 */

/**
 * Example: Minimal
 * Description:
 *   This example will shows the bare minimum required for IotWebConf to start up.
 *   After starting up the thing, please search for WiFi access points e.g. with
 *   your phone. Use password provided in the code!
 *   After connecting to the access point the root page will automatically appears.
 *   We call this "captive portal".
 *   
 *   Please set a new password for the Thing (for the access point) as well as
 *   the SSID and password of your local WiFi. You cannot move on without these steps.
 *   
 *   You have to leave the access point before to let the Thing continue operation
 *   with connecting to configured WiFi.
 *
 *   Note that you can find detailed debug information in the serial console depending
 *   on the settings IOTWEBCONF_DEBUG_TO_SERIAL, IOTWEBCONF_DEBUG_PWD_TO_SERIAL set
 *   in the IotWebConf.h .
 */

#include <IotWebConf.h>

// -- Initial name of the Thing. Used e.g. as SSID of the own Access Point.
const char thingName[] = "AutoconnectAP";

// -- Initial password to connect to the Thing, when it creates an own Access Point.
const char wifiInitialApPassword[] = "password";

DNSServer dnsServer;
WebServer server(80);

IotWebConf iotWebConf(thingName, &dnsServer, &server, wifiInitialApPassword);

void setup() 
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Starting up...");

  // -- Initializing the configuration.
  iotWebConf.init();

  // -- Set up required URL handlers on the web server.
  server.on("/", handleRoot);
  server.on("/config", []{ iotWebConf.handleConfig(); });
  server.onNotFound([](){ iotWebConf.handleNotFound(); });

  Serial.println("Ready.");
}

void loop() 
{
  // -- doLoop should be called as frequently as possible.
  iotWebConf.doLoop();
}

/**
 * Handle web requests to "/" path.
 */
void handleRoot()
{
  // -- Let IotWebConf test and handle captive portal requests.
  if (iotWebConf.handleCaptivePortal())
  {
    // -- Captive portal request were already served.
    return;
  }
  String s = "<!DOCTYPE html><html lang=\"en\"><head><meta name=\"viewport\" content=\"width=device-width, initial-scale=1, user-scalable=no\"/>";
  s += "<title>IotWebConf 01 Minimal</title></head><body>Hello world!<br>";
  s += "Go to <a href='config'>configure page</a> to change settings.";
  s += "</body></html>\n";

  server.send(200, "text/html", s);
}

//IotWebConf::IotWebConf(
//    const char* defaultThingName, DNSServer* dnsServer, WebServer* server,
//    const char* initialApPassword, const char* configVersion)
//{
//  strncpy(this->_thingName, defaultThingName, IOTWEBCONF_WORD_LEN);
//  this->_dnsServer = dnsServer;
//  this->_server = server;
//  this->_initialApPassword = initialApPassword;
//  this->_configVersion = configVersion;
//  itoa(this->_apTimeoutMs / 1000, this->_apTimeoutStr, 10);
//
//  this->_thingNameParameter = IotWebConfParameter("Thing name beep boop", "iwcThingName", this->_thingName, IOTWEBCONF_WORD_LEN);
//  this->_apPasswordParameter = IotWebConfParameter("AP password", "iwcApPassword", this->_apPassword, IOTWEBCONF_PASSWORD_LEN, "password");
//  this->_wifiSsidParameter = IotWebConfParameter("WiFi SSID", "iwcWifiSsid", this->_wifiSsid, IOTWEBCONF_WORD_LEN);
//  this->_wifiPasswordParameter = IotWebConfParameter("WiFi password", "iwcWifiPassword", this->_wifiPassword, IOTWEBCONF_PASSWORD_LEN, "password");
//  this->_apTimeoutParameter = IotWebConfParameter("Startup delay (seconds)", "iwcApTimeout", this->_apTimeoutStr, IOTWEBCONF_WORD_LEN, "number", NULL, NULL, "min='1' max='600'", false);
//  this->addParameter(&this->_thingNameParameter);
//  this->addParameter(&this->_apPasswordParameter);
//  this->addParameter(&this->_wifiSsidParameter);
//  this->addParameter(&this->_wifiPasswordParameter);
//  this->addParameter(&this->_apTimeoutParameter);
//}
