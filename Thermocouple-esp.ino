// this example is public domain. enjoy! https://learn.adafruit.com/thermocouple/

#include "max6675.h"

int thermoDO = 12;
int thermoCS = 15;
int thermoCLK = 14;

MAX6675 thermocouple(thermoCLK, thermoCS, thermoDO);

void setup() {
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  Serial.println("MAX6675 test");
  // wait for MAX chip to stabilize
  delay(500);
}

void loop() {
  float temperature = thermocouple.readCelsius();
  Serial.print("C = "); 
  Serial.println(temperature);
  
  if(temperature<25){
    digitalWrite(LED_BUILTIN, LOW);
  }else if(temperature>26){
    digitalWrite(LED_BUILTIN, HIGH);
  }
  // For the MAX6675 to update, you must delay AT LEAST 250ms between reads!
  delay(250);
}