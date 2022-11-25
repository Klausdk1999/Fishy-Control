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
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecureBearSSL.h>

const uint8_t fingerprint[20] = {0x40, 0xaf, 0x00, 0x6b, 0xec, 0x90, 0x22, 0x41, 0x8e, 0xa3, 0xad, 0xfa, 0x1a, 0xe8, 0x25, 0x41, 0x1d, 0x1a, 0x54, 0xb3};

ESP8266WiFiMulti WiFiMulti;

int thermoDO = 12;
int thermoCS = 15;
int thermoCLK = 14;

#define PIN_INPUT 0
#define RELAY_PIN 2

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


void setup() {
  Serial.begin(115200);

  windowStartTime = millis();

  //initialize the variables we're linked to
  Setpoint = 31;

  //tell the PID to range between 0 and the full window size
  myPID.SetOutputLimits(0, WindowSize);

  //turn the PID on
  myPID.SetMode(AUTOMATIC);

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("MAX6675 test");
  // wait for MAX chip to stabilize
  delay(500);

  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP("Minuano", "kf156873");

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

  if ((WiFiMulti.run() == WL_CONNECTED)) {

    std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

    //client->setFingerprint(fingerprint);
    // Or, if you happy to ignore the SSL certificate, then use the following line instead:
    client->setInsecure();

    HTTPClient https;

    Serial.print("[HTTPS] begin...\n");
    if (https.begin(*client, "https://fishy-control.vercel.app/api/data")) {  // HTTPS

      Serial.print("[HTTPS] POST...\n");
      https.addHeader("Content-Type", "application/json");

      String json = "{\"Temperature\":";
      json += thermocouple.readCelsius();
      json += ",\"Output\":";
      json += Output;
      json += " }";
      
      // start connection and send HTTP header
      int httpCode = https.POST(json);

      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTPS] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_MOVED_PERMANENTLY) {
          String payload = https.getString();
          Serial.println(payload);
        }
      } else {
        Serial.printf("[HTTPS] POST... failed, error: %s\n", https.errorToString(httpCode).c_str());
      }

      https.end();
    } else {
      Serial.printf("[HTTPS] Unable to connect\n");
    }
  }

  // Reconnect automatically
  // if(WiFi.status() != WL_CONNECTED){
  //   connectWiFi();
  // }else{
  //   WiFiClient client;
  //   HTTPClient http;
  //   String url="https://fishy-control.vercel.app/api/data";

  //   http.begin(client, url); //HTTP
  //   http.addHeader("Content-Type", "application/json");

  //   String json = "{\"Temperature\":";
  //   json += thermocouple.readCelsius();
  //   json += ",\"Output\":";
  //   json += Output;
  //   json += " }";
  //   int httpResponseCode = http.POST("{\"Temperature\":\"20.3\",\"Output\":62.5}");
     
  //   Serial.print("HTTP Response code: ");
  //   Serial.println(httpResponseCode);
        
  //   // Free resources
  //   http.end();
  // }
  
  
  server.handleClient();


  // For the MAX6675 to update, you must delay AT LEAST 250ms between reads!
  delay(250);
}