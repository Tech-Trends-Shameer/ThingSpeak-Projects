//Tech Trends Shameer
//Weather monitoring system with ESP8266 and Thingspeak.
 
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <DHT.h>
#include <SPI.h>
#include <Adafruit_BMP280.h>
#include <LiquidCrystal_I2C.h>

//Initialize the LCD display
LiquidCrystal_I2C lcd(0x27, 16, 2);


#define BMP_SCK  (13)
#define BMP_MISO (12)
#define BMP_MOSI (11)
#define BMP_CS   (10)

DHT dht(D3, DHT11);
//SFE_BMP180 bmp;
double T, P;
char status;
WiFiClient client;

Adafruit_BMP280 bmp; // I2C

String apiKey = "MGZDLWRFXGFFQAFX";
const char *ssid =  "WiFi Username";
const char *pass =  "WiFi Password";
const char* server = "api.thingspeak.com";


void setup() {
  Serial.begin(115200);
  delay(10);
  bmp.begin();
  Wire.begin();
  dht.begin();
  WiFi.begin(ssid, pass);


  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  lcd.begin();
  lcd.backlight();
  lcd.setCursor(3,0);
  lcd.print("Tech Trends  ");
  lcd.setCursor(0,0);
  delay(2000);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Weather Monitor");
  lcd.setCursor(4, 1);
  lcd.print("System");
  delay(4000);
  lcd.clear();


}



void loop() {

  //DHT11 Sensor Data
  float h = dht.readHumidity();
  float t = dht.readTemperature();

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }
  /*Serial.print("Temperature");
  Serial.println(t);
  Serial.print("Humidity");
  Serial.println(h);*/

  //BMP180 Sensor Data
   float temp = bmp.readTemperature();
  float pressure = bmp.readPressure();
  float alt = bmp.readAltitude(1013.25);

    if (!bmp.begin(0x76)) {
    Serial.println(F("Could not find a valid BMP280 sensor, check wiring!"));
    while (1);
  }

  /* Default settings from datasheet. */
  bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */

  //Serial.print(F("Pressure = "));
  //Serial.print(pressure); /* Adjusted to local forecast! */
  //Serial.println(" pa");

  
  //Rain sensor
  //int rain = analogRead(A0);
  //rain = map(rain, 0, 1024, 0, 100);
    int rainvalue = analogRead(A0);
    int rain = map(rainvalue, 400, 1023, 100, 0);


  lcd.setCursor(0, 0);
  lcd.print("T:");
  lcd.print(t);

  lcd.setCursor(8, 0);
  lcd.print("H:");
  lcd.print(h);

  lcd.setCursor(0, 1);
  lcd.print("P:");
  lcd.print(pressure);

  lcd.setCursor(11, 1);
  lcd.print("R:");
  lcd.print(rain);
  lcd.print(" ");


  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(t);
    postStr += "&field2=";
    postStr += String(h);
    postStr += "&field3=";
    postStr += String(pressure, 2);
    postStr += "&field4=";
    postStr += String(rain);
    postStr += "\r\n\r\n\r\n\r\n";

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n\n\n");
    client.print(postStr);

    Serial.print("Temperature: ");
    Serial.println(t);
    Serial.print("Humidity: ");
    Serial.println(h);
    Serial.print("Pressure: ");
    Serial.print(pressure, 2);
    Serial.println("pa");
    Serial.print("Rain ");
    Serial.println(rain);

  }
  client.stop();
  delay(1000);
}
