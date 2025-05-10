//Component   Pin Name  
//DHT11       D2
//LDR         D1
//Gas Sensor  A0
//--------------------------------------------------------------------------
//Blynk App Setup:
//Create a new project in the Blynk app.
//Add widgets:
//Value Display for V0 (Temperature)
//Value Display for V1 (Humidity)
//Value Display for V2 (Gas Sensor)
//LED Widget for V4 (Weather Status - Green/Red)
//LED Widget for V5 (Day/Night Status - Yellow/Blue)
//----------------------------------------------------------------------------
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>

// ==== Blynk and WiFi Credentials ====
char auth[] = "QVqf_WnlOrojS_vkggT-WqmWBlW8R60O";  // <-- Replace with your Blynk auth token
char ssid[] = "123456789";         // <-- Replace with your WiFi name
char pass[] = "123456789";     // <-- Replace with your WiFi password

// ==== Sensor Config ====
#define DHTPIN D2
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

#define LDR_PIN D1  // Digital pin connected to LDR
#define GAS_SENSOR_PIN A0  // Analog pin for Gas sensor

BlynkTimer timer;

void setup() {
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  dht.begin();

  pinMode(LDR_PIN, INPUT);  // LDR sensor connected to digital pin
  pinMode(GAS_SENSOR_PIN, INPUT); // Gas sensor connected to analog pin

  timer.setInterval(2000L, sendSensorData);  // Run every 2 seconds
}

void sendSensorData() {
  // Read DHT11 values
  float temp = dht.readTemperature();
  float hum = dht.readHumidity();

  // Read gas sensor (analog value)
  int gasValue = analogRead(GAS_SENSOR_PIN);

  // Read LDR value (digital HIGH/LOW for Day/Night)
  int ldrState = digitalRead(LDR_PIN);  // HIGH = Bright (Day), LOW = Dark (Night)
 
  Serial.print("ldr: ");
  Serial.println(ldrState);
  bool isDay = ldrState == HIGH;

  // Send sensor data to Blynk
  Blynk.virtualWrite(V0, temp);
  Blynk.virtualWrite(V1, hum);
  Blynk.virtualWrite(V2, gasValue);

  // ==== Weather Status Logic ====
  bool isWeatherNormal = (temp >= 15 && temp <= 35) &&
                         (hum >= 30 && hum <= 70) &&
                         (gasValue < 400);

  // Send Weather Status to Virtual LED in Blynk App (V4)
  Blynk.virtualWrite(V4, isWeatherNormal ?255:0); // Green (255) = Normal, Red (0) = Abnormal

  Serial.print("weather=");
  Serial.println(isWeatherNormal);
  // ==== Day/Night Detection (LDR) ====
  // Send Day/Night status to Virtual LED in Blynk App (V5)
  Blynk.virtualWrite(V3, isDay ? 0 : 1);  // 1 = Day (Yellow), 0 = Night (Blue)

  // ==== Serial Monitor Print ====
  Serial.println("================================");
  Serial.print("Temperature (C): ");
  Serial.println(temp);

  Serial.print("Humidity (%): ");
  Serial.println(hum);

  Serial.print("Gas Sensor Value: ");
  Serial.println(gasValue);

  Serial.print("It is: ");
  Serial.println(isDay ? "Daytime" : "Nighttime");

  Serial.print("Weather Status: ");
  Serial.println(isWeatherNormal ? "Normal (Green)" : "Abnormal (Red)");
  Serial.println("================================");
}

void loop() {
  Blynk.run();
  timer.run();
}
