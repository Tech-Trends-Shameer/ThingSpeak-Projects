//Tech Trends Shameer
//Air Quality Monitoring

#include <DHT.h>  // Including library for dht
#include "MQ135.h"
#include <ESP8266WiFi.h> 
 
String apiKey = "JN90N79YUFHJSN1VDQPE7";     //  Enter your Write API key from ThingSpeak
 
const char *ssid =  "WiFi Username";     // replace with your wifi ssid and wpa2 key
const char *pass =  "WiFi Password";
const char* server = "api.thingspeak.com";
const int sensorPin= 0;
int air_quality;

#define DHTPIN 4            //Connect to GPIO 2 in NodeMCU Board
 
DHT dht(DHTPIN, DHT11);
 
WiFiClient client;
 
void setup() 
{
       Serial.begin(115200);
       delay(10);
       dht.begin();
 
       Serial.println("Connecting to ");
       Serial.println(ssid);
 
 
       WiFi.begin(ssid, pass);
 
      while (WiFi.status() != WL_CONNECTED) 
     {
            delay(500);
            Serial.print(".");
     }
      Serial.println("");
      Serial.println("WiFi connected");
 
}
 
void loop() 
{
  
      float h = dht.readHumidity();
      float t = dht.readTemperature();
      //int gasValue = analogRead(gas);
      MQ135 gasSensor = MQ135(A0);
      air_quality = gasSensor.getPPM();
             if (client.connect(server,80))   //   "184.106.153.149" or api.thingspeak.com
                      {  
                            
                             String postStr = apiKey;
                             postStr +="&field1=";
                             postStr += String(t);
                             postStr +="&field2=";
                             postStr += String(h);
                             postStr +="&field3=";
                             postStr += String(air_quality);
                             postStr += "\r\n\r\n";
 
                             client.print("POST /update HTTP/1.1\n");
                             client.print("Host: api.thingspeak.com\n");
                             client.print("Connection: close\n");
                             client.print("X-THINGSPEAKAPIKEY: "+apiKey+"\n");
                             client.print("Content-Type: application/x-www-form-urlencoded\n");
                             client.print("Content-Length: ");
                             client.print(postStr.length());
                             client.print("\n\n");
                             client.print(postStr);
 
                             Serial.print("Temperature: ");
                             Serial.print(t);
                             Serial.print(" degrees Celcius, Humidity: ");
                             Serial.print(h);
                             //Serial.print("%, Gas Value: ");
                             //Serial.print(h);
                             Serial.print("%, Air Quality: ");
                             Serial.print(air_quality);
                             Serial.println("PPM. Send to Thingspeak.");
                        }
          client.stop();
 
          Serial.println("Waiting...");
  
  // thingspeak needs minimum 15 sec delay between updates
  delay(1000);
}
