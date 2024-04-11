#include <ArduinoJson.h>                          
#include <ArduinoJson.hpp>  
#include <WiFi.h>                                 
#include <PubSubClient.h>                         
#include <Wire.h>                                 
#include "DHT.h" 
#define DHTTYPE DHT11                            
#define DHTPIN 4

const char* ssid = "WLAN-ESP";           
const char* password = "agsesp32";    
const char* mqtt_server = "10.0.42.10";       
const char* topic = "Temperatur";              
const char* client_id = "Sensor";

WiFiClient wifi_client;                           
PubSubClient client(wifi_client);
DHT dht(DHTPIN, DHTTYPE);

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

  dht.begin();              
}

void loop() {
  if (!client.connected()) {                      
    reconnect();                                  
  }

  StaticJsonDocument<80> doc;                   
  char output[80];                              

  float temp = dht.readTemperature();           
  float humidity = dht.readHumidity();          
  
  doc["id"] = client_id;                        
  doc["t"] = temp;                              
  doc["h"] = humidity;                          

  serializeJson(doc, output);                   
  Serial.println(output);                       
  client.publish(topic, output);

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
