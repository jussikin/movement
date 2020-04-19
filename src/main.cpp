#include <Arduino.h>
#include <MLEDScroll.h>
#include "config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>
#define MOVEMENT_PIN D4
#define TIMOUT 60

char ssid[] = WIFIHOTSPOT;
char pw[] = WIFIKEY;

MLEDScroll matrix;
WiFiClient espClient;
PubSubClient client(espClient);
Ticker sekuntiTikuttaja;

int secudejaJaSusia = 0;



void letterBlink(String _letter, uint8_t _pause) {
  matrix.setIntensity(0);
  matrix.character(_letter);
  for(uint8_t i=0;i<=8;i++) {
    matrix.setIntensity(i);
    delay(_pause);  
  }
  delay(_pause*10);
  for(uint8_t i=8;i<=0;i--) {
    matrix.setIntensity(i);
    delay(_pause);  
  }   
}

void tickUp(){
  secudejaJaSusia++;
  if(secudejaJaSusia>TIMOUT){
    letterBlink("R",50);
    ESP.reset();
  }
}


void callback(char* topic, byte* payload, unsigned int length) {
 secudejaJaSusia=0;
}


void setupWifi(){
    WiFi.begin(WIFIHOTSPOT, WIFIKEY);
    while (WiFi.status() != WL_CONNECTED) {
      delay(500);
      Serial.print(".");
    }
    client.setServer(MQTTSERVER, 1883);
    client.setCallback(callback);
}

void setup() {
   matrix.begin();
   matrix.flip=true;
   letterBlink("S",50);
   Serial.begin(115200);
   sekuntiTikuttaja.attach(1,tickUp);
   setupWifi();
   pinMode(MOVEMENT_PIN,INPUT);
}




void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
      client.subscribe(CALLBACKTOPIC);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}


void loop() {

  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  if(digitalRead(MOVEMENT_PIN)){
   matrix.clear();
   client.publish(TOPIC,"1");
  }else {
    client.publish(TOPIC,"0");
    matrix.clear();
  }
  delay(300);
  client.loop();
  delay(300);
}