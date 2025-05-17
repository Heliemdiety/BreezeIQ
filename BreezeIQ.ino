#include <WiFi.h>
#include <WebServer.h>
#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <HTTPClient.h>
#include <Arduino.h>

// Constants
#define DHTPIN 4
#define DHTTYPE DHT22
#define OUTPUT_PIN 5  
#define BUTTON1 15
#define BUTTON2 16
#define BUTTON3 17

// WiFi Credentials
const char* ssid = "12345";
const char* password = "12345";
String pcServer = "http://YOUR_PC_IP:5000";  // Python server address

// Variables
DHT dht(DHTPIN, DHTTYPE);
WebServer server(80);
int activeUser = 0;
float temperature = 0;
float humidity = 0;
int fanSpeed = 0;

void setup() {
  Serial.begin(115200);
  

  pinMode(OUTPUT_PIN, OUTPUT);
  ledcSetup(0, 5000, 8);        
  ledcAttachPin(OUTPUT_PIN, 0);  
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  pinMode(BUTTON3, INPUT_PULLUP);
  dht.begin();

  // Connect to WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) delay(500);
  Serial.println("Connected! IP: " + WiFi.localIP().toString());

  // Web Server Routes
  server.on("/", handleRoot);
  server.begin();
}

void loop() {
  server.handleClient();
  readDHT();
  checkButtons();
  sendDataToPC();
  delay(100);
}

void readDHT() {
  float newTemp = dht.readTemperature();
  float newHum = dht.readHumidity();
  
  if (!isnan(newTemp)) temperature = newTemp;
  if (!isnan(newHum)) humidity = newHum;
}

void checkButtons() {
  static unsigned long lastDebounce = 0;
  if (millis() - lastDebounce < 200) return;
  
  int user = 0;
  if (digitalRead(BUTTON1) == LOW) user = 1;
  if (digitalRead(BUTTON2) == LOW) user = 2;
  if (digitalRead(BUTTON3) == LOW) user = 3;

  if (user != activeUser) {
    activeUser = user;
    lastDebounce = millis();
  }
}

void sendDataToPC() {
  if (WiFi.status() != WL_CONNECTED) return;
  
  HTTPClient http;
  String url = pcServer + "/update";
  http.begin(url);
  http.addHeader("Content-Type", "application/json");
  
  String payload = String("{\"user\":") + activeUser + 
                   ",\"temp\":" + temperature + 
                   ",\"hum\":" + humidity + "}";
                   
  int httpCode = http.POST(payload);
  
  if (httpCode == HTTP_CODE_OK) {
    String response = http.getString();
    fanSpeed = response.toInt();
    fanSpeed = constrain(fanSpeed, 0, 100);
    ledcWrite(0, map(fanSpeed, 0, 100, 0, 255));
  }
  
  http.end();
}

void handleRoot() {
  String html = R"(
  <html><head>
  <meta http-equiv='refresh' content='2'>
  <title>Fan Control Monitor</title>
  <style>
    body {font-family: Arial; padding: 20px;}
    .data {background: #f0f0f0; padding: 20px; margin: 10px;}
  </style>
  </head><body>
    <h1>Fan Control System</h1>
    <div class="data">
      <p>Temperature: )" + String(temperature) + R"(°C</p>
      <p>Humidity: )" + String(humidity) + R"(%</p>
      <p>Active User: User )" + String(activeUser) + R"(</p>
      <p>Fan Speed: )" + String(fanSpeed) + R"(%</p>
    </div>
  </body></html>
  )";
  
  server.send(200, "text/html", html);
}