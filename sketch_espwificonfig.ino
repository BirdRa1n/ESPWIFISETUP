#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <EEPROM.h>

// Constants
const int EEPROM_SIZE = 512;
const int SSID_START_ADDR = 0;
const int PASS_START_ADDR = 32;
const int SSID_LENGTH = 32;
const int PASS_LENGTH = 64;
const int BUTTON_RESET_PIN = D8;
const int MAX_WIFI_WAIT_TIME = 20000; // in milliseconds

const char* AP_SSID = "ESP8266";
const char* AP_PASS = ""; // Open network

ESP8266WebServer server(80);

// Function Declarations
bool connectToWiFi(const String& ssid, const String& pass);
void startAccessPoint();
void setupWebServer();
void handleNetworkInfo();
void handleNetworkList();
void handleSettings();
void handleRoot();
String readStringFromEEPROM(int startAddress, int length);
void writeStringToEEPROM(int startAddress, const String& data);
void restartESP();

// Setup Function
void setup() {
  Serial.begin(9600);
  Serial.println("Initializing...");

  // Initialize EEPROM
  EEPROM.begin(EEPROM_SIZE);

  // Check if the reset button is pressed
  if (digitalRead(BUTTON_RESET_PIN) == HIGH) {
    startAccessPoint();
    setupWebServer();
    server.begin();
    Serial.println("Access Point started.");
    return;
  }

  // Read SSID and password from EEPROM
  String ssid = readStringFromEEPROM(SSID_START_ADDR, SSID_LENGTH);
  String pass = readStringFromEEPROM(PASS_START_ADDR, PASS_LENGTH);

  // Attempt to connect to WiFi
  if (connectToWiFi(ssid, pass)) {
    Serial.println("Successfully connected to WiFi.");
    setupWebServer();
    server.begin();
  } else {
    Serial.println("Failed to connect to WiFi. Starting Access Point.");
    startAccessPoint();
    setupWebServer();
    server.begin();
  }
}

// Loop Function
void loop() {
  server.handleClient();
  if (WiFi.isConnected()) {
    Serial.println("Connected! IP address: " + WiFi.localIP().toString());
    delay(5000); // Adjust as needed
  } else {
    restartESP();
  }
}

// Functions Implementation
bool connectToWiFi(const String& ssid, const String& pass) {
  WiFi.begin(ssid.c_str(), pass.c_str());
  unsigned long startMillis = millis();
  while (millis() - startMillis < MAX_WIFI_WAIT_TIME) {
    if (WiFi.status() == WL_CONNECTED) {
      return true;
    }
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnection timed out.");
  return false;
}

void startAccessPoint() {
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  Serial.print("Access Point started. IP address: ");
  Serial.println(WiFi.softAPIP());
}

void setupWebServer() {
  server.on("/networkinfo", HTTP_GET, handleNetworkInfo);
  server.on("/networks", HTTP_GET, handleNetworkList);
  server.on("/setting", HTTP_POST, handleSettings);
  server.on("/", HTTP_GET, handleRoot);
}

void handleNetworkInfo() {
  String ssid = WiFi.SSID();
  int rssi = WiFi.RSSI();

  DynamicJsonDocument json(256);
  json["ssid"] = ssid;
  json["rssi"] = rssi;

  String content;
  serializeJson(json, content);
  server.send(200, "application/json", content);
}

void handleNetworkList() {
  int numNetworks = WiFi.scanNetworks();
  DynamicJsonDocument json(1024);
  JsonArray networks = json.createNestedArray("networks");

  for (int i = 0; i < numNetworks; i++) {
    JsonObject network = networks.createNestedObject();
    network["ssid"] = WiFi.SSID(i);
    network["rssi"] = WiFi.RSSI(i);
    network["encryption"] = WiFi.encryptionType(i);
  }

  String content;
  serializeJson(json, content);
  server.send(200, "application/json", content);
}

void handleSettings() {
  String ssid = server.arg("ssid");
  String pass = server.arg("pass");

  DynamicJsonDocument json(256);
  if (!ssid.isEmpty() && !pass.isEmpty()) {
    writeStringToEEPROM(SSID_START_ADDR, ssid);
    writeStringToEEPROM(PASS_START_ADDR, pass);
    EEPROM.commit();

    json["message"] = "WiFi credentials saved. Restarting...";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", serializeJsonString(json));
    delay(5000); // Allow time for message to be sent
    restartESP();
  } else {
    json["message"] = "Invalid credentials.";
    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(400, "application/json", serializeJsonString(json));
  }
}

void handleRoot() {
  DynamicJsonDocument json(256);
  json["msg"] = "ok";
  String content;
  serializeJson(json, content);
  server.send(200, "application/json", content);
}

String readStringFromEEPROM(int startAddress, int length) {
  String data;
  for (int i = 0; i < length; ++i) {
    char c = EEPROM.read(startAddress + i);
    if (c == '\0') break; // Stop at null terminator
    data += c;
  }
  return data;
}

void writeStringToEEPROM(int startAddress, const String& data) {
  for (int i = 0; i < data.length(); ++i) {
    EEPROM.write(startAddress + i, data[i]);
  }
  EEPROM.write(startAddress + data.length(), '\0'); // Null terminator
}

void restartESP() {
  Serial.println("Restarting ESP...");
  ESP.restart();
}

String serializeJsonString(const DynamicJsonDocument& doc) {
  String result;
  serializeJson(doc, result);
  return result;
}
