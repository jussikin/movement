#include <Arduino.h>
#include <MLEDScroll.h>
#include "config.h"
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <Ticker.h>
void tickUp();
#define MOVEMENT_PIN D4
#define TIMOUT 60

char ssid[] = WIFIHOTSPOT;
char pw[] = WIFIKEY;

MLEDScroll matrix;
WiFiClient espClient;
PubSubClient client(espClient);
Ticker sekuntiTikuttaja(tickUp, 1000, 0, MILLIS);

int secudejaJaSusia = 0;
int lastValue=3;



void tickUp(){
  secudejaJaSusia++;
  if(secudejaJaSusia>TIMOUT){
    matrix.character("R");
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
   matrix.character("S");
   Serial.begin(115200);
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
  int read = digitalRead(MOVEMENT_PIN);
  if(read!=lastValue){
   matrix.clear();
   if(read==0){
      client.publish(TOPIC,"0");
      matrix.character("O");
   } else {
      client.publish(TOPIC,"1");
      matrix.character("M");
   }
   lastValue=read;
  }
  delay(300);
  client.loop();
  delay(300);
}