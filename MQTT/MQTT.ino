#include <ArduinoJson.h>                          
#include <ArduinoJson.hpp>  
#include <WiFi.h>                                 
#include <PubSubClient.h>                         
#include <Wire.h>  
#include <OneWire.h>
#include <DallasTemperature.h>                               

// Data wire is conntec to the Arduino digital pin 4
#define ONE_WIRE_BUS 4

// Setup a oneWire instance to communicate with any OneWire devices
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature sensor 
DallasTemperature sensors(&oneWire);

const char* ssid = "WLAN-ESP";           
const char* password = "agsesp32";    
const char* mqtt_server = "10.0.42.12";       
const char* topic = "Temperatur";              
const char* client_id = "Sensor";

WiFiClient wifi_client;                           
PubSubClient client(wifi_client);

void setup() {
  Serial.begin(115200);                           
  while (!Serial);                                

  Serial.println();                               
  Serial.print("Connecting to ");                 
  Serial.println(ssid);

  WiFi.begin(ssid, password);                     

  while (WiFi.status() != WL_CONNECTED) {         
    delay(500);                                   
    Serial.print(".");                            
  }  

  Serial.println("");                             
  Serial.println("WiFi connected");               
  Serial.println("IP address: ");                 
  Serial.println(WiFi.localIP());   

  client.setServer(mqtt_server, 1883);  

  sensors.begin();
}

void loop() {
  if (!client.connected()) {                      
    reconnect();                                  
  }
  sensors.requestTemperatures();
  
  float tempC = sensors.getTempCByIndex(0);      
  float tempF = sensors.getTempFByIndex(0);

  char tempCString[8];
  char tempFString[8];
  
  dtostrf(tempC, 6, 2, tempCString);
  dtostrf(tempF, 6, 2, tempFString);

  Serial.println(tempCString);
  Serial.println(tempFString);
  
  client.publish("Temperature_Celsius", tempCString);
  client.publish("Temperature_Fahrenheit", tempFString);

  delay(2000);
}

void reconnect() {                                
  while (!client.connected()) {                   
    Serial.print("Attempting MQTT connection...");
    String clientId = "ESP32Client-";             
    clientId += String(random(0xffff), HEX);      
    if (client.connect(clientId.c_str())) {
      Serial.println("connected");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      delay(5000);
    }
  }
}
