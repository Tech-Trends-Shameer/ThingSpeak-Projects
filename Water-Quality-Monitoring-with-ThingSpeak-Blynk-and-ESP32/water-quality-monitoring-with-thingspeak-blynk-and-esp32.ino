#define BLYNK_TEMPLATE_ID "TMPL3bQHcYr6I"
#define BLYNK_TEMPLATE_NAME "Water Quality Monitoring"
#define BLYNK_AUTH_TOKEN "c08Y6MdoeHBwFltofui6UsdkdKCjIpVDEquoPpC"

// Comment this out to disable prints and save space
#define BLYNK_PRINT Serial
#include <BlynkSimpleEsp32.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <LiquidCrystal_I2C.h>

// LCD configuration
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Your WiFi credentials.
char ssid[] = "WiFi Username";
char pass[] = "WiFi Password";

char auth[] = BLYNK_AUTH_TOKEN;

String apiKey = "ThingSpeak Write API Key";  // Enter your Write API key from ThingSpeak
const char *server = "api.thingspeak.com";
WiFiClient client;

namespace pin {
    const byte tds_sensor = 34;
}

namespace device {
    float aref = 3.3; // Vref, this is for 3.3v compatible controller boards, for Arduino use 5.0v.
}

namespace sensor {
    float ec = 0;
    unsigned int tds = 0;
    float ecCalibration = 1;
}

void sendToThingSpeak(float tds, float ec) {
    if (client.connect(server, 80)) {
        String postStr = apiKey;
        postStr += "&field1=";
        postStr += String(tds);
        postStr += "&field2=";
        postStr += String(ec);
        postStr += "\r\n\r\n";

        client.print("POST /update HTTP/1.1\n");
        client.print("Host: api.thingspeak.com\n");
        client.print("Connection: close\n");
        client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
        client.print("Content-Type: application/x-www-form-urlencoded\n");
        client.print("Content-Length: ");
        client.print(postStr.length());
        client.print("\n\n");
        client.print(postStr);

        Serial.print("TDS: ");
        Serial.print(tds);
        Serial.print(" ppm, EC: ");
        Serial.print(ec);
        Serial.println(" mS/cm. Sent to ThingSpeak.");
    }
    client.stop();
}

void readTdsQuick() {
    // Read the raw analog value and convert to voltage
    float rawEc = analogRead(pin::tds_sensor) * device::aref / 1024.0;

    // Debugging: Print the raw analog value
    Serial.print(F("Raw Analog Value: "));
    Serial.println(rawEc);

    // Adjust this offset based on the sensor's dry reading (without immersion)
    float offset = 0.14; // Set this to the observed raw analog value in air

    // Apply calibration and offset compensation
    sensor::ec = (rawEc * sensor::ecCalibration) - offset;

    // If the EC is below zero after adjustment, set it to zero
    if (sensor::ec < 0) sensor::ec = 0;

    // Convert voltage value to TDS value using a cubic equation
    sensor::tds = (133.42 * pow(sensor::ec, 3) - 255.86 * sensor::ec * sensor::ec + 857.39 * sensor::ec) * 0.5;

    // Debugging: Print the TDS and EC values
    Serial.print(F("TDS: "));
    Serial.println(sensor::tds);
    Serial.print(F("EC: "));
    Serial.println(sensor::ec, 2);

    lcd.setCursor(0, 0);
    lcd.print("TDS: ");
    lcd.setCursor(4, 0);
    lcd.print(sensor::tds);

    lcd.setCursor(0, 1);
    lcd.print("EC: ");
    lcd.setCursor(4, 1);
    lcd.print(sensor::ec);

    // Send data to Blynk virtual pins
    Blynk.virtualWrite(V0, sensor::tds);
    Blynk.virtualWrite(V1, sensor::ec);

    // Send data to ThingSpeak
    sendToThingSpeak(sensor::tds, sensor::ec);
}

void setup() {
    Serial.begin(115200); // Debugging on hardware Serial 0
    Blynk.begin(auth, ssid, pass);

    // Initialize LCD
    lcd.init();
    lcd.backlight();

    // Display "Tech Trends Shameer"
    lcd.setCursor(3, 0);
    lcd.print("Tech Trends ");
    lcd.setCursor(3, 1);
    lcd.print("  Shameer    ");
    delay(2000);  // Wait for 2 seconds

    // Clear the LCD and display "Water Quality Monitoring"
    lcd.clear();
    lcd.setCursor(1, 0);
    lcd.print("Water Quality");
    lcd.setCursor(3, 1);
    lcd.print("Monitoring   ");
    delay(2000);
    lcd.clear();
}

void loop() {
    Blynk.run();
    readTdsQuick();
    delay(1000);
}
