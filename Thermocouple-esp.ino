// this example is public domain. enjoy! https://learn.adafruit.com/thermocouple/

#include "max6675.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

int thermoDO = 12;
int thermoCS = 15;
int thermoCLK = 14;

#define DEFAULT_SSID "Minuano"     // Default Wifi SSID
#define DEFAULT_KEY "kf156873"      // Default Wifi WPA2-PSK

#define SSID_LEN 32             // Max SSID length as 802.11 definition
#define KEY_LEN 63              // Max WPA2-PSK length

ESP8266WebServer server(80);

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);


/**
 * HTTP route handlers
 ********************************************************************************/

/**
 * GET /
 */
void handleGETRoot() 
{
  // I always loved this HTTP code
  server.send(418, "text/plain", "\
            _           \r\n\
         _,(_)._            \r\n\
    ___,(_______).          \r\n\
  ,'__.           \\    /\\_  \r\n\
 /,' /             \\  /  /  \r\n\
| | |              |,'  /   \r\n\
 \\`.|                  /    \r\n\
  `. :           :    /     \r\n\
    `.            :.,'      \r\n\
      `-.________,-'        \r\n\
  \r\n");
}

/**
 * GET /debug
 */
void handleGETDebug()
{
  String msg = "lalala" ;//logger.getLog();
  
  if(!isAuthBasicOK())
    return;

  Serial.print(msg);
  server.send(200, "text/plain", msg);
}

/**
 * GET /temperature
 */
void handleGETTemp()
{
  if(!isAuthBasicOK())
  return;
  
  String json = "{ temperature:";
  json += thermocouple.readCelsius();
  json += " }\r\n";
  server.send(200, "application/json", json);
}

/**
 * WEB helpers 
 ********************************************************************************/

bool isAuthBasicOK()
{
  // Disable auth if not credential provided
  // if(strlen(settings.login) > 0 && strlen(settings.password) > 0)
  // {
  //   if(!server.authenticate(settings.login, settings.password))
  //   {
  //     server.requestAuthentication();
  //     return false;
  //   }
  // }
  return true;
}

void connectWiFi()
{
  WiFi.begin(DEFAULT_SSID, DEFAULT_KEY);
  Serial.print("Connecting to WiFi ");
  Serial.println(DEFAULT_SSID);
  // Wait for connection
  while(WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("OK");

  // Display local ip
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP().toString());
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("MAX6675 test");
  // wait for MAX chip to stabilize
  delay(500);

  WiFi.mode(WIFI_STA);

  // Setup HTTP handlers
  server.on("/", handleGETRoot );
  server.on("/temperature", HTTP_GET, handleGETTemp);
  // server.on("/state", HTTP_GET, handleGETState);
  // server.on("/state", HTTP_POST, handlePOSTState);
  // server.on("/settings", HTTP_GET, handleGETSettings);
  // server.on("/settings", HTTP_POST, handlePOSTSettings);
  server.onNotFound([]() {
    server.send(404, "text/plain", "Not found");
  });
  server.begin();
  Serial.println("HTTP server started.");

}

void loop() {
  float temperature = thermocouple.readCelsius();
  Serial.print("C = "); 
  Serial.println(temperature);
  
  // Reconnect automatically
  if(WiFi.status() != WL_CONNECTED)
    connectWiFi();
  
  server.handleClient();

  if(temperature<25){
    digitalWrite(LED_BUILTIN, LOW);
  }else if(temperature>26){
    digitalWrite(LED_BUILTIN, HIGH);
  }
  // For the MAX6675 to update, you must delay AT LEAST 250ms between reads!
  delay(250);
}