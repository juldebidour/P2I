#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "AutoConnectAP";
const char* password = "password";

ESP8266WebServer server(80);

void handleRoot() {
  String content = "<html><body>";
  content += "<h1>Welcome to ESP8266 Server</h1>";
  content += "<p>Temperature: " + String(getTemperature()) + " &deg;C</p>";
  content += "<p>Pressure: " + String(getPressure()) + " Pa</p>";
  content += "<p>Particle Count: " + String(getParticleCount()) + "</p>";
  content += "</body></html>";

  server.send(200, "text/html", content);
}

float getTemperature() {
  // Replace this with your BMP280 temperature reading logic
  return 25.5;
}

float getPressure() {
  // Replace this with your BMP280 pressure reading logic
  return 101325.0;
}

int getParticleCount() {
  // Replace this with your SDS011 particle count reading logic
  return 50;
}

void setup() {
  Serial.begin(115200);

  // Connect to WiFi
  WiFi.softAP(ssid, password);
  Serial.println("WiFi AP Started");

  // Define server routes
  server.on("/", HTTP_GET, handleRoot);

  // Start server
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Handle incoming client requests
  server.handleClient();
}
