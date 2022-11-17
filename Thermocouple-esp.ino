// this example is public domain. enjoy! https://learn.adafruit.com/thermocouple/
/********************************************************
 * PID RelayOutput Example
 * Same as basic example, except that this time, the output
 * is going to a digital pin which (we presume) is controlling
 * a relay.  the pid is designed to Output an analog value,
 * but the relay can only be On/Off.
 *
 *   to connect them together we use "time proportioning
 * control"  it's essentially a really slow version of PWM.
 * first we decide on a window size (5000mS say.) we then
 * set the pid to adjust its output between 0 and that window
 * size.  lastly, we add some logic that translates the PID
 * output into "Relay On Time" with the remainder of the
 * window being "Relay Off Time"
 ********************************************************/

#include <PID_v1.h>
#include "max6675.h"
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>

int thermoDO = 12;
int thermoCS = 15;
int thermoCLK = 14;

#define PIN_INPUT 0
#define RELAY_PIN 2

#define DEFAULT_SSID "Minuano"     // Default Wifi SSID
#define DEFAULT_KEY "kf156873"      // Default Wifi WPA2-PSK

#define SSID_LEN 32             // Max SSID length as 802.11 definition
#define KEY_LEN 63              // Max WPA2-PSK length

ESP8266WebServer server(80);

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

//Define Variables we'll be connecting to
double Setpoint, Input, Output;

//Specify the links and initial tuning parameters
double Kp=2, Ki=5, Kd=1;
PID myPID(&Input, &Output, &Setpoint, Kp, Ki, Kd, DIRECT);

int WindowSize = 5000;
unsigned long windowStartTime;

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
 * GET /temperature
 */
void handleGETTemp()
{
  if(!isAuthBasicOK())
  return;

  String json = "{\"Temperature\":";
  json += thermocouple.readCelsius();
  json += ",\"PID_Output\":";
  json += Output;
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

  windowStartTime = millis();

  //initialize the variables we're linked to
  Setpoint = 34;

  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("MAX6675 test");
  // wait for MAX chip to stabilize
  delay(500);

  WiFi.mode(WIFI_STA);

  // Setup HTTP handlers
  server.on("/", handleGETRoot );
  server.on("/temperature", HTTP_GET, handleGETTemp);
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

  Input = temperature;// analogRead(PIN_INPUT);
  myPID.Compute();

  if (millis() - windowStartTime > WindowSize)
  { //time to shift the Relay Window
    windowStartTime += WindowSize;
  }
  if (Output < millis() - windowStartTime){
    digitalWrite(RELAY_PIN, HIGH);
    digitalWrite(LED_BUILTIN, HIGH);
  } 
  else{ 
    digitalWrite(RELAY_PIN, LOW);
    digitalWrite(LED_BUILTIN, LOW);
  }
  //if (Output < millis() - windowStartTime) digitalWrite(RELAY_PIN, HIGH);
  //else digitalWrite(RELAY_PIN, LOW);

  Serial.print("Output = "); 
  Serial.println(Output);

  // Reconnect automatically
  if(WiFi.status() != WL_CONNECTED)
  connectWiFi();
  
  server.handleClient();

  // if(temperature<25){
  //   digitalWrite(LED_BUILTIN, LOW);
  // }else if(temperature>26){
  //   digitalWrite(LED_BUILTIN, HIGH);
  // }

  // For the MAX6675 to update, you must delay AT LEAST 250ms between reads!
  delay(250);
}