#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BME280.h>

// Define your WiFi credentials
const char *ssid = "Airpure Station";
const char *password = "password";

// Create a BME280 sensor object
Adafruit_BME280 bme;

// Create an ESP8266WebServer object
ESP8266WebServer server(80);

void handleRoot() {
  String content = "<html><head><title>The Airpure Project</title>\n";
  content += "<meta charset=\"UTF-8\">\n";
  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  content += "<style>";
  content += "body {font-family: Arial, sans-serif; margin: 0; padding: 0;}";
  content += ".w3-top {position: sticky; top: 0; z-index: 9999;}";
  content += ".w3-bar {overflow: hidden; background-color: #1e3d59;}";
  content += ".w3-bar a {float: left; display: block; color: white; text-align: center; padding: 14px 16px; text-decoration: none;}";
  content += ".w3-bar a:hover {background-color: #ddd; color: black;}";
  content += ".w3-bar a.active {background-color: #f5f0e1; color: black;}";
  content += ".description {display: none;}"; // Style pour cacher le texte de description par défaut
  content += "</style>\n";
  content += "</head><body>\n";
  content += "<div class=\"w3-top\">\n";
  content += "<div class=\"w3-bar w3-card\">\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button\">The Airpure Project</a>\n";
  content += "<a href=\"/weather\" class=\"w3-bar-item w3-button\">The Airpure Weather Station</a>\n";
  content += "<a href=\"/data\" class=\"w3-bar-item w3-button\">Data</a>\n";
  content += "<a href=\"/specifications\" class=\"w3-bar-item w3-button\">Specifications</a>\n";
  content += "<a href=\"/codes\" class=\"w3-bar-item w3-button\">Codes</a>\n";
  content += "</div>\n";
  content += "<header class=\"w3-container w3-red w3-center\" style=\"padding:128px 16px\">\n";
  content += "<h1 class=\"w3-margin w3-jumbo\">The Airpure Project</h1>\n";
  content += "<p class=\"w3-xlarge\">Welcome to the Airpure Project !</p>\n";
  content += "<button onclick=\"toggleDescription()\" class=\"w3-button w3-black w3-padding-large w3-large w3-margin-top\" style=\"color: #ff6e40;\">See more information!</button>\n";
  content += "</header>\n";
  content += "<div class=\"w3-row-padding w3-padding-64 w3-container\">";
  content += "<div class=\"w3-content\"><div class=\"w3-twothird description\"><h1>The Airpure Project in brief :</h1>\n"; // Ajout de la classe "description"
  content += "<h5 class=\"w3-padding-32\">The Airpure Project aims to provide accurate and up-to-date weather information using the Airpure Weather Station.\nThe station is equipped with high-precision sensors to monitor temperature, humidity, air pressure, and more.\nStay informed about current weather conditions and trends with the Airpure Project !</h5></div></div></div></div>\n";
  content += "<div style=\"text-align: center; margin-top: 20px;\">";
  content += "<p>Visit my Github page for more informations :</p>";
  content += "<a href=\"https://github.com/juldebidour/P2I\"><button style=\"padding: 10px 20px; background-color: #1e3d59; color: white; border: none; cursor: pointer; border-radius: 5px;\">Link to my Github page</button></a>";
  content += "<p>Visit the ENSC website for more informations :</p>";
  content += "<a href=\"https://ensc.bordeaux-inp.fr/fr\"><button style=\"padding: 10px 20px; background-color: #1e3d59; color: white; border: none; cursor: pointer; border-radius: 5px;\">Link to the ENSC Website</button></a>";
  content += "</div>\n\n\n";
  content += "<script>"; // Début du script JavaScript
  content += "function toggleDescription() {"; // Définition de la fonction
  content += "var description = document.querySelector('.description');"; // Sélection de l'élément avec la classe "description"
  content += "if (description.style.display === 'none') {"; // Vérifie si le texte est caché
  content += "description.style.display = 'block';"; // Affiche le texte
  content += "} else {";
  content += "description.style.display = 'none';"; // Cache le texte
  content += "}";
  content += "}";
  content += "</script>"; // Fin du script JavaScript
  content += "</body></html>";

  server.send(200, "text/html", content);
}



void handleWeather() {
  // Read sensor data
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F; // Convert pressure to hPa

  String content = "<html><head><title>The Airpure Weather Station</title>\n";
  content += "<meta charset=\"UTF-8\">\n";
  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  content += "<script src=\"https://cdnjs.cloudflare.com/ajax/libs/Chart.js/3.5.1/chart.min.js\"></script>\n";
  content += "<style>";
  content += "body {font-family: Arial, sans-serif; margin: 0; padding: 0;}";
  content += ".w3-top {position: sticky; top: 0; z-index: 9999;}";
  content += ".w3-bar {overflow: hidden; background-color: #1e3d59;}";
  content += ".w3-bar a {float: left; display: block; color: white; text-align: center; padding: 14px 16px; text-decoration: none;}";
  content += ".w3-bar a:hover {background-color: #ddd; color: black;}";
  content += ".w3-bar a.active {background-color: #f5f0e1; color: black;}";
  content += "</style>\n";
  content += "</head><body>\n";
  content += "<div class=\"w3-top\">\n";
  content += "<div class=\"w3-bar w3-card\">\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button\">The Airpure Project</a>\n";
  content += "<a href=\"/weather\" class=\"w3-bar-item w3-button active\">The Airpure Weather Station</a>\n";
  content += "<a href=\"/data\" class=\"w3-bar-item w3-button\">Data</a>\n";
  content += "<a href=\"/specifications\" class=\"w3-bar-item w3-button\">Specifications</a>\n";
  content += "<a href=\"/codes\" class=\"w3-bar-item w3-button\">Codes</a>\n";
  content += "</div>\n";
  content += "</div>\n";
  content += "<header class=\"w3-container w3-red w3-center\" style=\"padding:128px 16px\">\n";
  content += "<h1 class=\"w3-margin w3-jumbo\">The Airpure Weather Station</h1>\n";
  content += "<p class=\"w3-xlarge\">Current weather conditions</p>\n";
  content += "</header>\n";
  content += "<div class=\"w3-row-padding w3-padding-64 w3-container\">\n";
  content += "<div class=\"w3-content\"><div class=\"w3-twothird\"><h1>Real-time Data:</h1>\n";
  content += "<canvas id=\"realtime-chart\"></canvas>\n";
  content += "<script>\n";
  content += "var ctx = document.getElementById('realtime-chart').getContext('2d');\n";
  content += "var chart = new Chart(ctx, {\n";
  content += "type: 'bar',\n";
  content += "data: {\n";
  content += "labels: ['Temperature', 'Humidity', 'Pressure'],\n";
  content += "datasets: [{\n";
  content += "label: 'Real-time Data',\n";
  content += "data: [" + String(temperature) + ", " + String(humidity) + ", " + String(pressure) + "],\n";
  content += "backgroundColor: [\n";
  content += "'rgba(255, 99, 132, 0.2)',\n";
  content += "'rgba(54, 162, 235, 0.2)',\n";
  content += "'rgba(255, 206, 86, 0.2)'\n";
  content += "],\n";
  content += "borderColor: [\n";
  content += "'rgba(255, 99, 132, 1)',\n";
  content += "'rgba(54, 162, 235, 1)',\n";
  content += "'rgba(255, 206, 86, 1)'\n";
  content += "],\n";
  content += "borderWidth: 1\n";
  content += "}]\n";
  content += "},\n";
  content += "options: {\n";
  content += "scales: {\n";
  content += "y: {\n";
  content += "beginAtZero: true\n";
  content += "}\n";
  content += "}\n";
  content += "}\n";
  content += "});\n";
  content += "</script>\n";
  content += "</div></div></div></div></body></html>\n";
  String footer = "<footer style=\"background-color: #1e3d59; padding: 20px; text-align: center;\">"
                  "<p><span style=\"color: white;\">Visit my Github page for more informations :</span></p>"
                  "<a href=\"https://github.com/juldebidour/P2I\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Link to my Github page</button></a>"
                  "<p<span style=\"color: white;\">Visit the ENSC website for more informations :</span></p>"
                  "<a href=\"https://ensc.bordeaux-inp.fr/fr\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Link to the ENSC Website</button></a>"
                  "</footer>\n";

  content += footer;
  server.send(200, "text/html", content);
}

void handleData() {
  String content = "<html><head><title>Data from the Airpure Station</title>\n";
  content += "<meta charset=\"UTF-8\">\n";
  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  content += "<style>";
  content += "body {font-family: Arial, sans-serif; margin: 0; padding: 0;}";
  content += ".w3-top {position: sticky; top: 0; z-index: 9999;}";
  content += ".w3-bar {overflow: hidden; background-color: #1e3d59;}";
  content += ".w3-bar a {float: left; display: block; color: white; text-align: center; padding: 14px 16px; text-decoration: none;}";
  content += ".w3-bar a:hover {background-color: #ddd; color: black;}";
  content += ".w3-bar a.active {background-color: #f5f0e1; color: black;}";
  content += "</style>\n";
  content += "</head><body>\n";
  content += "<div class=\"w3-top\">\n";
  content += "<div class=\"w3-bar w3-card\">\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button\">The Airpure Project</a>\n";
  content += "<a href=\"/weather\" class=\"w3-bar-item w3-button\">The Airpure Weather Station</a>\n";
  content += "<a href=\"/data\" class=\"w3-bar-item w3-button active\">Data</a>\n";
  content += "<a href=\"/specifications\" class=\"w3-bar-item w3-button\">Specifications</a>\n";
  content += "<a href=\"/codes\" class=\"w3-bar-item w3-button\">Codes</a>\n";
  content += "</div>\n";
  content += "</div>\n";
  content += "<header class=\"w3-container w3-red w3-center\" style=\"padding:128px 16px\">\n";
  content += "<h1 class=\"w3-margin w3-jumbo\">Data from the Airpure Station</h1>\n";
  content += "<p class=\"w3-xlarge\">Explore the data collected by the Airpure Station !</p>\n";
  content += "<button class=\"w3-button w3-black w3-padding-large w3-large w3-margin-top\" onclick=\"toggleCode()\" style=\"color: #ff6e40;\">See more information!</button>\n";
  content += "</header>\n";
  content += "<div class=\"w3-row-padding w3-padding-64 w3-container\">\n";
  content += "<div class=\"w3-content\"><div class=\"w3-twothird\"><h1>Data from the Airpure Station in brief :</h1>\n";
  content += "<h5 class=\"w3-padding-32\">The Airpure Station collects various types of data related to air quality, weather conditions, and environmental parameters.\nThis includes measurements of particulate matter (PM2.5 and PM10), concentrations of gases such as CO2 and VOCs, temperature, humidity, pressure, and more.\nThe collected data is stored and analyzed to provide insights into air pollution levels, weather patterns, and trends over time. Users can access the data through a web-based interface, allowing them to monitor real-time conditions and make informed decisions to protect their health and the environment.</h5></div></div></div></div></body></html>\n";
  String footer = "<footer style=\"background-color: #1e3d59; padding: 20px; text-align: center;\">"
                  "<p><span style=\"color: white;\">Visit my Github page for more informations :</span></p>"
                  "<a href=\"https://github.com/juldebidour/P2I\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Link to my Github page</button></a>"
                  "<p><span style=\"color: white;\">Visit the ENSC website for more informations :</span></p>"
                  "<a href=\"https://ensc.bordeaux-inp.fr/fr\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Link to the ENSC Website</button></a>"
                  "</footer>\n";

  content += footer;
  server.send(200, "text/html", content);
}

void handleSpecifications() {
  String content = "<html><head><title>Specifications of the Airpure Station</title>\n";
  content += "<meta charset=\"UTF-8\">\n";
  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  content += "<style>";
  content += "body {font-family: Arial, sans-serif; margin: 0; padding: 0;}";
  content += ".w3-top {position: sticky; top: 0; z-index: 9999;}";
  content += ".w3-bar {overflow: hidden; background-color: #1e3d59;}";
  content += ".w3-bar a {float: left; display: block; color: white; text-align: center; padding: 14px 16px; text-decoration: none;}";
  content += ".w3-bar a:hover {background-color: #ddd; color: black;}";
  content += ".w3-bar a.active {background-color: #f5f0e1; color: black;}";
  content += "</style>\n";
  content += "</head><body>\n";
  content += "<div class=\"w3-top\">\n";
  content += "<div class=\"w3-bar w3-card\">\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button\">The Airpure Project</a>\n";
  content += "<a href=\"/weather\" class=\"w3-bar-item w3-button\">The Airpure Weather Station</a>\n";
  content += "<a href=\"/data\" class=\"w3-bar-item w3-button\">Data</a>\n";
  content += "<a href=\"/specifications\" class=\"w3-bar-item w3-button active\">Specifications</a>\n";
  content += "<a href=\"/codes\" class=\"w3-bar-item w3-button\">Codes</a>\n";
  content += "</div>\n";
  content += "</div>\n";
  content += "<header class=\"w3-container w3-red w3-center\" style=\"padding:128px 16px\">\n";
  content += "<h1 class=\"w3-margin w3-jumbo\">Specifications of the Airpure Station</h1>\n";
  content += "<p class=\"w3-xlarge\">Get to know more about the technical specifications of the Airpure Station !</p>\n";
  content += "<button class=\"w3-button w3-black w3-padding-large w3-large w3-margin-top\" onclick=\"toggleCode()\" style=\"color: #ff6e40;\">See more information!</button>\n";
  content += "</header>\n";
  content += "<div class=\"w3-row-padding w3-padding-64 w3-container\">\n";
  content += "<div class=\"w3-content\"><div class=\"w3-twothird\"><h1>Specifications of the Airpure Station in brief :</h1>\n";
  content += "<h5 class=\"w3-padding-32\">The Airpure Station is equipped with state-of-the-art sensors and technologies to monitor air quality and weather conditions with high accuracy and reliability.\nSome of the key specifications include :</h5>\n";
  content += "<ul class=\"w3-ul w3-card-4\">\n";
  content += "<li class=\"w3-padding-16\"> Measurement of particulate matter (PM2.5 and PM10) using SDS 011 sensor</li>\n";
  content += "<li class=\"w3-padding-16\"> Detection of gases such as CO2 and VOCs with MQ135 sensor</li>\n";
  content += "<li class=\"w3-padding-16\"> Monitoring of temperature, humidity, and pressure with BME280 sensor</li>\n";
  content += "<li class=\"w3-padding-16\"> Real-time data transmission to remote server via Wi-Fi</li>\n";
  content += "<li class=\"w3-padding-16\"> User-friendly web interface for data visualization and analysis</li>\n";
  content += "</ul></div></div></div></div></body></html>\n";
  String footer = "<footer style=\"background-color: #1e3d59; padding: 20px; text-align: center;\">"
                  "<p><span style=\"color: white;\">Visit my Github page for more informations :</span></p>"
                  "<a href=\"https://github.com/juldebidour/P2I\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Link to my Github page</button></a>"
                  "<p><span style=\"color: white;\">Visit the ENSC website for more informations :</span></p>"
                  "<a href=\"https://ensc.bordeaux-inp.fr/fr\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Link to the ENSC Website</button></a>"
                  "</footer>\n";

  content += footer;
  server.send(200, "text/html", content);
}


void handleCodes() {
  String content = "<html><head><title>Codes of the Airpure Station</title>\n";
  content += "<meta charset=\"UTF-8\">\n";
  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  content += "<style>";
  content += "body {font-family: Arial, sans-serif; margin: 0; padding: 0;}";
  content += ".w3-top {position: sticky; top: 0; z-index: 9999;}";
  content += ".w3-bar {overflow: hidden; background-color: #1e3d59;}";
  content += ".w3-bar a {float: left; display: block; color: white; text-align: center; padding: 14px 16px; text-decoration: none;}";
  content += ".w3-bar a:hover {background-color: #ddd; color: black;}";
  content += ".w3-bar a.active {background-color: #f5f0e1; color: black;}";
  content += ".code-container {display: none;}";
  content += "</style>\n";
  content += "<script>\n";
  content += "function toggleCode() {\n";
  content += "var x = document.getElementById('code-container');\n";
  content += "if (x.style.display === 'none') {\n";
  content += "x.style.display = 'block';\n";
  content += "} else {\n";
  content += "x.style.display = 'none';\n";
  content += "}\n";
  content += "}\n";
  content += "</script>\n";
  content += "</head><body>\n";
  content += "<div class=\"w3-top\">\n";
  content += "<div class=\"w3-bar w3-card\">\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button\">The Airpure Project</a>\n";
  content += "<a href=\"/weather\" class=\"w3-bar-item w3-button\">The Airpure Weather Station</a>\n";
  content += "<a href=\"/data\" class=\"w3-bar-item w3-button\">Data</a>\n";
  content += "<a href=\"/specifications\" class=\"w3-bar-item w3-button\">Specifications</a>\n";
  content += "<a href=\"/codes\" class=\"w3-bar-item w3-button active\" onclick=\"toggleCode()\">Codes</a>\n";
  content += "</div>\n";
  content += "<header class=\"w3-container w3-red w3-center\" style=\"padding:128px 16px\">\n";
  content += "<h1 class=\"w3-margin w3-jumbo\">Codes of the Airpure Station</h1>\n";
  content += "<p class=\"w3-xlarge\">Explore the codes behind the Airpure Station !</p>\n";
  content += "<button class=\"w3-button w3-black w3-padding-large w3-large w3-margin-top\" onclick=\"toggleCode()\" style=\"color: #ff6e40;\">See more information!</button>\n";
  content += "</header>\n";
  content += "<div id=\"code-container\" class=\"w3-row-padding w3-padding-64 w3-container code-container\">\n";
  content += "<div class=\"w3-content\"><div class=\"w3-twothird\"><h1>Codes of the Airpure Station in brief :</h1>\n";
  content += "<h5 class=\"w3-padding-32\">The Airpure Station is powered by Arduino-compatible microcontrollers and programmed using the Arduino IDE.\nIt utilizes various libraries and sensor modules to collect, process, and transmit data.\nThe main components of the station's code include :</h5>\n";
  content += "<ul class=\"w3-ul w3-card-4\">\n";
  content += "<li class=\"w3-padding-16\"> Integration of BME280 library for temperature, humidity, and pressure sensing</li>\n";
  content += "<li class=\"w3-padding-16\"> Utilization of SDS011 library for particulate matter measurement</li>\n";
  content += "<li class=\"w3-padding-16\"> Implementation of MQTT protocol for data transmission over Wi-Fi</li>\n";
  content += "<li class=\"w3-padding-16\"> Development of web server using ESP8266WebServer library for hosting data</li>\n";
  content += "<li class=\"w3-padding-16\"> Creation of dynamic web pages with HTML, CSS, and JavaScript for data visualization</li>\n";
  content += "</ul>\n";
  content += "<h2>Full Code :</h2>\n";
  // Ajoutez ici le code complet
  content += "<pre><code>";
  // Ajoutez ici le code complet
  content += "</code></pre>";
  content += "</div></div></div></div></body></html>\n";
  String footer = "<footer style=\"background-color: #1e3d59; padding: 20px; text-align: center;\">"
                  "<p><span style=\"color: white;\">Visit my Github page for more informations :</span></p>"
                  "<a href=\"https://github.com/juldebidour/P2I\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Link to my Github page</button></a>"
                  "<p><span style=\"color: white;\">Visit the ENSC website for more informations :</span></p>"
                  "<a href=\"https://ensc.bordeaux-inp.fr/fr\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Link to the ENSC Website</button></a>"
                  "</footer>\n";

  content += footer;
  server.send(200, "text/html", content);
}

void setup() {
  Serial.begin(115200);

  // Connecter au WiFi
  WiFi.softAP(ssid, password);
  Serial.println("WiFi AP Started");

  // Initialize BME280 sensor
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
  }

  // Définir les routes du serveur
  server.on("/", HTTP_GET, handleRoot);
  server.on("/weather", HTTP_GET, handleWeather);
  server.on("/data", HTTP_GET, handleData);
  server.on("/specifications", HTTP_GET, handleSpecifications);
  server.on("/codes", HTTP_GET, handleCodes);

  // Démarrer le serveur
  server.begin();
  Serial.println("HTTP server started");
}

void loop() {
  // Gérer les requêtes client
  server.handleClient();
}