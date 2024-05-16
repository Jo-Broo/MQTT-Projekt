// Es werden diverse Biblotheken eingebunden 
// für z.B WLAN-Kommunikation, Sensordaten auslesen
#include <WiFi.h>                                 
#include <PubSubClient.h>                         
#include <Wire.h>  
#include <OneWire.h>
#include <DallasTemperature.h>                               

// Sensor Setup
#define ONE_WIRE_BUS 4 
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// Hier werden einige Daten festgelegt die für die WLAN bzw MQTT verbindungen wichtig sind
const char* ssid = "WLAN-ESP";           
const char* password = "agsesp32";    
const char* mqtt_server = "10.0.42.12";       

// WLAN, MQTT Setup
WiFiClient wifi_client;                           
PubSubClient client(wifi_client);

void setup() {
  // Die Serielle Kommunikation wird zum debuggen verwendet
  Serial.begin(115200);                           
  while (!Serial);                                

  Serial.println();                               
  Serial.print("Connecting to ");                 
  Serial.println(ssid);

  // Die WLAN-Verbindung wird aufgebaut
  WiFi.begin(ssid, password);                     
  while (WiFi.status() != WL_CONNECTED) {         
    delay(500);                                   
    Serial.print(".");                            
  }  

  Serial.println("");                             
  Serial.println("WiFi connected");               
  Serial.println("IP address: ");                 
  Serial.println(WiFi.localIP());   

  // Die MQTT-Broker Informationen werden gesetzt 
  client.setServer(mqtt_server, 1883);  

  // Der Sensorbus wird gestartet 
  sensors.begin();
}

void loop() {
  // hier wird geschaut ob die Verbindung zum MQTT-Broker 
  // noch besteht
  if (!client.connected()) {                      
    reconnect();                                  
  }

  // es werden die neuen Temperaturen gemessen
  sensors.requestTemperatures();
  
  // in entsprechende variablen gespeichert
  // und in zeichenketten zum übertragen umgewandelt
  float tempC = sensors.getTempCByIndex(0);      
  char tempCString[8];
  dtostrf(tempC, 6, 2, tempCString);
  Serial.println(tempCString);

  float tempF = sensors.getTempFByIndex(0);
  char tempFString[8];
  dtostrf(tempF, 6, 2, tempFString);
  Serial.println(tempFString);

  // Die Daten werden auf den jeweiligen Topics veröffentlicht
  client.publish("Temperature_Celsius", tempCString);
  client.publish("Temperature_Fahrenheit", tempFString);

  // Der Messvorgang wird um 2s verzögert
  delay(2000);
}

// Diese Methode baut die Verbindung zum MQTT-Broker auf
void reconnect() {                                
  while (!client.connected()) {                   
    Serial.print("Attempting MQTT connection...");
    // Der Client erhält bei jedem Verbindungsversuch eine neue einzigartige ID damit keine Konflikte 
    // mit bereits bestehenden Clients auftreten
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
