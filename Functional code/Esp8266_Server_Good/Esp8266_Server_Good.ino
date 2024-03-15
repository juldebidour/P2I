#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <MQ135.h>
#include <Adafruit_BME280.h>
#include <NovaSDS011.h>
#include <RTClib.h>

// Define your WiFi credentials
const char *ssid = "Airpure Station";
const char *password = "password";

// Define pin for the buzzer
#define BUZZER_PIN 8 // Use any available GPIO pin

#define SDS_PIN_RX 2
#define SDS_PIN_TX 3

NovaSDS011 sds011;

#define SEALEVELPRESSURE_HPA (1013.25)

Adafruit_BME280 bme; // I2C
// Adafruit_BME280 bme(BME_CS); // hardware SPI
// Adafruit_BME280 bme(BME_CS, BME_MOSI, BME_MISO, BME_SCK); // software SPI

#define PIN_MQ135 A0 // MQ135 Analog Input Pin

MQ135 mq135_sensor(PIN_MQ135);

float temperature, humidity, seaLevel; // Temp and Humid floats, will be measured by the DHT

unsigned long delayTime;

int capteur_lum = 0; // capteur branché sur le port 0
int analog_lum; // valeur analogique envoyé par le capteur

int readD1;
int readD2;
int Pin_D1 = 4;
int Pin_D2 = 5;

// Create an ESP8266WebServer object
ESP8266WebServer server(80);

String username = "one";

RTC_DS3231 rtc;



void handleRoot() {
  // Lire l'heure actuelle du module RTC
  DateTime now = rtc.now();

  // Construire la chaîne représentant l'heure actuelle avec les secondes
  String timeString = String(now.hour()) + ":" + 
                      String(now.minute()) + ":" + 
                      String(now.second());

  // Construire la chaîne représentant la date actuelle
  String dateString = String(now.day()) + "/" + 
                      String(now.month()) + "/" + 
                      String(now.year());

  String content = "<html>\n";
  content += "<head>\n";
  content += "<title>The Airpure Project</title>\n";
  content += "<meta charset=\"UTF-8\">\n";
  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  content += "<style>\n";
  content += "body {\n";
  content += "  font-family: Arial, sans-serif;\n";
  content += "  margin: 0;\n";
  content += "  padding: 0;\n";
  content += "}\n";
  content += ".w3-top {\n";
  content += "  position: sticky;\n";
  content += "  top: 0;\n";
  content += "  z-index: 9999;\n";
  content += "}\n";
  content += ".w3-bar {\n";
  content += "  overflow: hidden;\n";
  content += "  background-color: #1e3d59;\n";
  content += "}\n";
  content += ".w3-bar a {\n";
  content += "  float: left;\n";
  content += "  display: block;\n";
  content += "  color: white;\n";
  content += "  text-align: center;\n";
  content += "  padding: 14px 16px;\n";
  content += "  text-decoration: none;\n";
  content += "}\n";
  content += ".w3-bar a:hover {\n";
  content += "  background-color: #ddd;\n";
  content += "  color: black;\n";
  content += "}\n";
  content += ".w3-bar a.active {\n";
  content += "  background-color: #f5f0e1;\n";
  content += "  color: black;\n";
  content += "}\n";
  content += ".footer {\n";
  content += "  background-color: #1e3d59;\n";
  content += "  padding: 20px;\n";
  content += "  text-align: center;\n";
  content += "  color: white;\n";
  content += "}\n";
  content += ".content {\n";
  content += "  padding: 20px;\n";
  content += "}\n";
  content += ".more-info {\n";
  content += "  display: none;\n";
  content += "}\n";
  content += "</style>\n";
  content += "<script>\n";
  content += "function showMoreInfo() {\n";
  content += "  var x = document.getElementById('moreInfo');\n";
  content += "  if (x.style.display === 'none') {\n";
  content += "    x.style.display = 'block';\n";
  content += "  } else {\n";
  content += "    x.style.display = 'none';\n";
  content += "  }\n";
  content += "}\n";
  content += "function updateTime() {\n";
  content += "  var currentTime = new Date();\n";
  content += "  var hours = currentTime.getHours();\n";
  content += "  var minutes = currentTime.getMinutes();\n";
  content += "  var seconds = currentTime.getSeconds();\n";
  content += "  var timeString = hours + ':' + (minutes < 10 ? '0' : '') + minutes + ':' + (seconds < 10 ? '0' : '') + seconds;\n";
  content += "  document.getElementById('current-time').innerText = timeString;\n";
  content += "  setTimeout(updateTime, 1000); // Mettre à jour chaque seconde\n";
  content += "}\n";
  content += "window.onload = function() {\n";
  content += "  updateTime();\n";
  content += "}\n";
  content += "</script>\n"; // Fermez correctement la balise script ici
  content += "</head>\n";
  content += "<body>\n";
  content += "<div class=\"w3-top\">\n";
  content += "<div class=\"w3-bar w3-card\">\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button active\">The Airpure Project</a>\n";
  content += "<a href=\"/weather\" class=\"w3-bar-item w3-button\">Weather Station</a>\n";
  content += "<a href=\"/data\" class=\"w3-bar-item w3-button\">Data</a>\n";
  content += "<a href=\"/specifications\" class=\"w3-bar-item w3-button\">Specifications</a>\n";
  content += "<a href=\"/codes\" class=\"w3-bar-item w3-button\">Codes</a>\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button\" style=\"color: white; float: right;\"> Welcome to our dear user " + String(username) +  " !</a>\n";
  content += "<a href=\"/login\" class=\"w3-bar-item w3-button\" style=\"float: right;\">Login</a>\n";
  // Ajouter la balise span pour afficher l'heure
  content += "<span id=\"current-time\" class=\"w3-bar-item\" style=\"color: white; float: right; padding: 14px 16px;\"></span>\n";
  content += "</div>\n";
  content += "<header class=\"w3-container w3-red w3-center\" style=\"padding:32px 16px\">\n";
  content += "<h1 class=\"w3-margin w3-xxlarge\">The Airpure Project</h1>\n";
  content += "<p class=\"w3-large\">Welcome to the Airpure Project!</p>\n";
  content += "<button onclick=\"showMoreInfo()\" class=\"w3-button w3-black w3-padding-large w3-large\" style=\"margin-top: 32px;\">See More Information</button>\n";
  content += "</header>\n";
  content += "<div class=\"content\">\n";
  content += "<div id=\"moreInfo\" class=\"more-info\">\n";
  content += "<p>The Airpure Project aims to provide accurate and up-to-date weather information using the Airpure Weather Station.\nThe station is equipped with high-precision sensors to monitor temperature, humidity, air pressure, and more.\nStay informed about current weather conditions and trends with the Airpure Project !</p>\n";
  content += "</div>\n";
  content += "</div>\n";
  content += "<div class=\"footer\">\n";
  content += "<p>For more information:</p>\n";
  content += "<a href=\"https://github.com/juldebidour/P2I\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Visit my Github page</button></a>\n";
  content += "<a href=\"https://ensc.bordeaux-inp.fr/fr\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Visit the ENSC Website</button></a>\n";
  content += "</div>\n";
  content += "</body>\n";
  content += "</html>\n";

  server.send(200, "text/html", content);
}



void handleLogin() {
  // Lire l'heure actuelle du module RTC
  DateTime now = rtc.now();

  // Construire la chaîne représentant l'heure actuelle avec les secondes
  String timeString = String(now.hour()) + ":" + 
                      String(now.minute()) + ":" + 
                      String(now.second());

  // Construire la chaîne représentant la date actuelle
  String dateString = String(now.day()) + "/" + 
                      String(now.month()) + "/" + 
                      String(now.year());

  String content = "<html>\n";
  content += "<head>\n";
  content += "<title>The Airpure Project</title>\n";
  content += "<meta charset=\"UTF-8\">\n";
  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  content += "<style>\n";
  content += "body {\n";
  content += "  font-family: Arial, sans-serif;\n";
  content += "  margin: 0;\n";
  content += "  padding: 0;\n";
  content += "}\n";
  content += ".w3-top {\n";
  content += "  position: sticky;\n";
  content += "  top: 0;\n";
  content += "  z-index: 9999;\n";
  content += "}\n";
  content += ".w3-bar {\n";
  content += "  overflow: hidden;\n";
  content += "  background-color: #1e3d59;\n";
  content += "}\n";
  content += ".w3-bar a {\n";
  content += "  float: left;\n";
  content += "  display: block;\n";
  content += "  color: white;\n";
  content += "  text-align: center;\n";
  content += "  padding: 14px 16px;\n";
  content += "  text-decoration: none;\n";
  content += "}\n";
  content += ".w3-bar a:hover {\n";
  content += "  background-color: #ddd;\n";
  content += "  color: black;\n";
  content += "}\n";
  content += ".w3-bar a.active {\n";
  content += "  background-color: #f5f0e1;\n";
  content += "  color: black;\n";
  content += "}\n";
  content += ".footer {\n";
  content += "  background-color: #1e3d59;\n";
  content += "  padding: 20px;\n";
  content += "  text-align: center;\n";
  content += "  color: white;\n";
  content += "}\n";
  content += ".content {\n";
  content += "  padding: 20px;\n";
  content += "}\n";
  content += ".more-info {\n";
  content += "  display: none;\n";
  content += "}\n";
  content += "</style>\n";
  content += "<script>\n";
  content += "function showMoreInfo() {\n";
  content += "  var x = document.getElementById('moreInfo');\n";
  content += "  if (x.style.display === 'none') {\n";
  content += "    x.style.display = 'block';\n";
  content += "  } else {\n";
  content += "    x.style.display = 'none';\n";
  content += "  }\n";
  content += "}\n";
  content += "function updateTime() {\n";
  content += "  var currentTime = new Date();\n";
  content += "  var hours = currentTime.getHours();\n";
  content += "  var minutes = currentTime.getMinutes();\n";
  content += "  var seconds = currentTime.getSeconds();\n";
  content += "  var timeString = hours + ':' + (minutes < 10 ? '0' : '') + minutes + ':' + (seconds < 10 ? '0' : '') + seconds;\n";
  content += "  document.getElementById('current-time').innerText = timeString;\n";
  content += "  setTimeout(updateTime, 1000); // Mettre à jour chaque seconde\n";
  content += "}\n";
  content += "window.onload = function() {\n";
  content += "  updateTime();\n";
  content += "}\n";
  content += "</script>\n";
  content += "</head>\n";
  content += "<body>\n";
  content += "<div class=\"w3-top\">\n";
  content += "<div class=\"w3-bar w3-card\">\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button active\">The Airpure Project</a>\n";
  content += "<a href=\"/weather\" class=\"w3-bar-item w3-button\">Weather Station</a>\n";
  content += "<a href=\"/data\" class=\"w3-bar-item w3-button\">Data</a>\n";
  content += "<a href=\"/specifications\" class=\"w3-bar-item w3-button\">Specifications</a>\n";
  content += "<a href=\"/codes\" class=\"w3-bar-item w3-button\">Codes</a>\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button\" style=\"color: white; float: right;\"> Welcome to our dear user " + String(username) +  " !</a>\n";
  content += "<a href=\"/login\" class=\"w3-bar-item w3-button\" style=\"float: right;\">Login</a>\n";
  // Ajouter la balise span pour afficher l'heure
  content += "<span id=\"current-time\" class=\"w3-bar-item\" style=\"color: white; float: right; padding: 14px 16px;\">Current Time: " + timeString + "</span>\n";
  content += "</div>\n";
  content += "<div class=\"login-container\">\n";
  content += "<h2>Login page</h2>\n";
  content += "<form class=\"login-form\" action=\"/login\" method=\"POST\">\n";
  content += "<label for=\"username\">Username :</label><br>\n";
  content += "<input type=\"text\" id=\"username\" name=\"username\" onchange=\"updateUsername()\"><br>\n"; 
  content += "<label for=\"password\">Password :</label><br>\n";
  content += "<input type=\"password\" id=\"password\" name=\"password\"><br><br>\n";
  content += "<a href=\"/\" class=\"w3-button w3-block w3-teal\">Login</a>\n";
  content += "</form>\n";
  content += "</div>\n";
  content += "</body>\n";
  content += "</html>\n";
  String footer = "<footer style=\"background-color: #1e3d59; padding: 20px; text-align: center;\">"
                  "<p><span style=\"color: white;\">Visit my Github page for more informations :</span></p>"
                  "<a href=\"https://github.com/juldebidour/P2I\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Link to my Github page</button></a>"
                  "<p><span style=\"color: white;\">Visit the ENSC website for more informations :</span></p>"
                  "<a href=\"https://ensc.bordeaux-inp.fr/fr\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Link to the ENSC Website</button></a>"
                  "</footer>\n";

  content += footer;
  
  // Send the HTML response
  server.send(200, "text/html", content);
}




void handleWeather() {
  float p25, p10;

  // Read sensor data
  float temperature = bme.readTemperature();
  float humidity = bme.readHumidity();
  float pressure = bme.readPressure() / 100.0F; // Convert pressure to hPa
  float altitude = bme.readAltitude(seaLevel);

  // Check if any reads failed and exit early (to try again).
  if (isnan(humidity) || isnan(temperature)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  readD1 = analogRead1(); // Read Analog value of first sensor
  delay(200);
  readD2 = analogRead2(); // Read Analog value of second sensor
  // Lire l'heure actuelle du module RTC
  DateTime now = rtc.now();

  // Construire la chaîne représentant l'heure actuelle avec les secondes
  String timeString = String(now.hour()) + ":" +
                      String(now.minute()) + ":" +
                      String(now.second());

  // Construire la chaîne représentant la date actuelle
  String dateString = String(now.day()) + "/" +
                      String(now.month()) + "/" +
                      String(now.year());

  //for first sensor
  analog_lum = 10 * readD1; //Read Analog value of first sensor

  digitalWrite(Pin_D1, LOW);
  digitalWrite(Pin_D2, HIGH); //Turn D2 On
  delay(100);
  float rzero = mq135_sensor.getRZero();
  float correctedRZero = mq135_sensor.getCorrectedRZero(temperature, humidity);
  float resistance = mq135_sensor.getResistance();
  float ppm = 10 * mq135_sensor.getPPM();
  float correctedPPM = 10 * mq135_sensor.getCorrectedPPM(temperature, humidity);
  delay(100);

  String content = "<html>\n";
  content += "<head>\n";
  content += "<title>The Airpure Weather Station</title>\n";
  content += "<meta charset=\"UTF-8\">\n";
  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  content += "<style>\n";
  content += "body {\n";
  content += "  font-family: Arial, sans-serif;\n";
  content += "  margin: 0;\n";
  content += "  padding: 0;\n";
  content += "}\n";
  content += ".w3-top {\n";
  content += "  position: sticky;\n";
  content += "  top: 0;\n";
  content += "  z-index: 9999;\n";
  content += "}\n";
  content += ".w3-bar {\n";
  content += "  overflow: hidden;\n";
  content += "  background-color: #1e3d59;\n";
  content += "}\n";
  content += ".w3-bar a {\n";
  content += "  float: left;\n";
  content += "  display: block;\n";
  content += "  color: white;\n";
  content += "  text-align: center;\n";
  content += "  padding: 14px 16px;\n";
  content += "  text-decoration: none;\n";
  content += "}\n";
  content += ".w3-bar a:hover {\n";
  content += "  background-color: #ddd;\n";
  content += "  color: black;\n";
  content += "}\n";
  content += ".w3-bar a.active {\n";
  content += "  background-color: #f5f0e1;\n";
  content += "  color: black;\n";
  content += "}\n";
  content += ".weather-container {\n";
  content += "  padding: 20px;\n";
  content += "}\n";
  content += ".weather-info {\n";
  content += "  margin-bottom: 20px;\n";
  content += "}\n";
  content += ".footer {\n";
  content += "  background-color: #1e3d59;\n";
  content += "  padding: 20px;\n";
  content += "  text-align: center;\n";
  content += "  color: white;\n";
  content += "}\n";
  content += "</style>\n";
  content += "<script>\n";
  content += "function showMoreInfo() {\n";
  content += "  var x = document.getElementById('moreInfo');\n";
  content += "  if (x.style.display === 'none') {\n";
  content += "    x.style.display = 'block';\n";
  content += "  } else {\n";
  content += "    x.style.display = 'none';\n";
  content += "  }\n";
  content += "}\n";
  content += "function updateTime() {\n";
  content += "  var currentTime = new Date();\n";
  content += "  var hours = currentTime.getHours();\n";
  content += "  var minutes = currentTime.getMinutes();\n";
  content += "  var seconds = currentTime.getSeconds();\n";
  content += "  var timeString = hours + ':' + (minutes < 10 ? '0' : '') + minutes + ':' + (seconds < 10 ? '0' : '') + seconds;\n";
  content += "  document.getElementById('current-time').innerText = timeString;\n";
  content += "  setTimeout(updateTime, 1000); // Mettre à jour chaque seconde\n";
  content += "}\n";
  content += "window.onload = function() {\n";
  content += "  updateTime();\n";
  content += "}\n";
  content += "</script>\n";
  content += "</head>\n";
  content += "<body>\n";
  content += "<div class=\"w3-top\">\n";
  content += "<div class=\"w3-bar w3-card\">\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button\">The Airpure Project</a>\n";
  content += "<a href=\"/weather\" class=\"w3-bar-item w3-button active\">The Airpure Weather Station</a>\n";
  content += "<a href=\"/data\" class=\"w3-bar-item w3-button\">Data</a>\n";
  content += "<a href=\"/specifications\" class=\"w3-bar-item w3-button\">Specifications</a>\n";
  content += "<a href=\"/codes\" class=\"w3-bar-item w3-button\">Codes</a>\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button\" style=\"color: white; float: right;\"> Welcome to our dear user " + String(username) + " !</a>\n";
  content += "<a href=\"/login\" class=\"w3-bar-item w3-button\" style=\"float: right;\">Login</a>\n";
  // Ajouter la balise span pour afficher l'heure
  content += "<span id=\"current-time\" class=\"w3-bar-item\" style=\"color: white; float: right; padding: 14px 16px;\">Current Time: " + timeString + "</span>\n";
  content += "</div>\n";
  content += "</div>\n";
  content += "<header class=\"w3-container w3-red w3-center\" style=\"padding:32px 16px\">\n";
  content += "<h1 class=\"w3-margin w3-xxlarge\">The Airpure Weather Station</h1>\n";
  content += "<h2 class=\"w3-large w3-animate-left\">Current weather conditions in the air</h2>\n";
  content += "<p> Date: " + dateString + " - Time: " + timeString + "</p>\n"; // Afficher la date et l'heure
  content += "<ul class=\"w3-ul w3-card-4\">\n";
  content += "<li class=\"w3-padding-16\"> \nQuantity of carbon dioxide (part per million) from the MQ-135 sensor :</li>\n\n";
  content += "<br><table border=\"<1\">\n";
  content += "<tr><td>Rzero</td><td>Resistance</td><td>PPM</td><td>Corrected PPM</td></tr>\n";
  content += "<tr><td>" + String(rzero) + "</td><td>" + String(resistance) + "</td><td>" + String(ppm) + "</td><td>" + String(correctedPPM / 3) + "</td></tr>\n";
  content += "</table>\n\n";
  content += "<br><li class=\"w3-padding-16\"> Quantity of microparticles (length 2.5 micro-meters and 10 2.5 micro-meters) from the SDS-011 sensor :</li>\n\n";
  content += "<br><table border=\"1\">\n";
  content += "<tr><td>Length (micro-meters)</td><td>2.5 10 micro-meters</td><td>10 micro-meters</td></tr>\n";
  content += "<tr><td>Value</td><td>" + String(p25) + "</td><td>" + String(p10) + "</td></tr>\n";
  content += "</table>\n\n";
  content += "<br><li class=\"w3-padding-16\"> Level of luminosity from the photoresistor (LDR) :</li>\n\n";
  content += "<br><table border=\"1\">\n";
  content += "<tr><td>Port of the sensor</td><td>Luminosity (lux)</td></tr>\n";
  content += "<tr><td>" + String(capteur_lum) + "</td><td>" + String(analog_lum) + "</td></tr>\n";
  content += "</table>\n\n";
  content += "<br><li class=\"w3-padding-16\"> Level of humidity, pressure, temperature, and altitude from the BME-280</li>\n\n";
  content += "<br><table border=\"1\">\n";
  content += "<tr><td>Humidity</td><td>Pressure</td><td>temperature</td><td>altitude</td></tr>\n";
  content += "<tr><td>" + String(humidity) + "</td><td>" + String(pressure) + "</td><td>" + String(temperature) + "</td><td>" + String(altitude) + "</td></tr>\n";
  content += "</table>\n\n";
  content += "</ul>\n";
  content += "</header>\n";
  content += "<div class=\"weather-container\">\n";
  content += "<div class=\"weather-info\">\n";
  content += "<h2>Real-time Data :</h2>\n";
  content += "<canvas id=\"realtime-chart\" width=\"400\" height=\"400\"></canvas>\n";
  content += "<script>\n";
  content += "var ctx = document.getElementById('realtime-chart').getContext('2d');\n";
  content += "var chart = new Chart(ctx, {\n";
  content += "  type: 'bar',\n";
  content += "  data: {\n";
  content += "    labels: ['Temperature', 'Humidity', 'Pressure'],\n";
  content += "    datasets: [{\n";
  content += "      label: 'Real-time Data',\n";
  // content += "      data: [" + String(temperature) + ", " + String(humidity) + ", " + String(pressure) + "],\n";
  content += "</div>\n";
  content += "</div>\n";
  content += "</body>\n";
  content += "</html>\n";
  String footer = "<footer style=\"background-color: #1e3d59; padding: 20px; text-align: center;\">"
                  "<p><span style=\"color: white;\">Visit my Github page for more informations :</span></p>"
                  "<a href=\"https://github.com/juldebidour/P2I\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Link to my Github page</button></a>"
                  "<p><span style=\"color: white;\">Visit the ENSC website for more informations :</span></p>"
                  "<a href=\"https://ensc.bordeaux-inp.fr/fr\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Link to the ENSC Website</button></a>"
                  "</footer>\n";

  content += footer;

  server.send(200, "text/html", content);
}


void handleData() {
  // Lire l'heure actuelle du module RTC
  DateTime now = rtc.now();

  // Construire la chaîne représentant l'heure actuelle avec les secondes
  String timeString = String(now.hour()) + ":" + 
                      String(now.minute()) + ":" + 
                      String(now.second());

  // Construire la chaîne représentant la date actuelle
  String dateString = String(now.day()) + "/" + 
                      String(now.month()) + "/" + 
                      String(now.year());

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
  content += "<script>\n";
  content += "function showMoreInfo() {\n";
  content += "  var x = document.getElementById('moreInfo');\n";
  content += "  if (x.style.display === 'none') {\n";
  content += "    x.style.display = 'block';\n";
  content += "  } else {\n";
  content += "    x.style.display = 'none';\n";
  content += "  }\n";
  content += "}\n";
  content += "function updateTime() {\n";
  content += "  var currentTime = new Date();\n";
  content += "  var hours = currentTime.getHours();\n";
  content += "  var minutes = currentTime.getMinutes();\n";
  content += "  var seconds = currentTime.getSeconds();\n";
  content += "  var timeString = hours + ':' + (minutes < 10 ? '0' : '') + minutes + ':' + (seconds < 10 ? '0' : '') + seconds;\n";
  content += "  document.getElementById('current-time').innerText = timeString;\n";
  content += "  setTimeout(updateTime, 1000); // Mettre à jour chaque seconde\n";
  content += "}\n";
  content += "window.onload = function() {\n";
  content += "  updateTime();\n";
  content += "}\n";
  content += "</script>\n";
  content += "</head><body>\n";
  content += "<div class=\"w3-top\">\n";
  content += "<div class=\"w3-bar w3-card\">\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button\">The Airpure Project</a>\n";
  content += "<a href=\"/weather\" class=\"w3-bar-item w3-button\">The Airpure Weather Station</a>\n";
  content += "<a href=\"/data\" class=\"w3-bar-item w3-button active\">Data</a>\n";
  content += "<a href=\"/specifications\" class=\"w3-bar-item w3-button\">Specifications</a>\n";
  content += "<a href=\"/codes\" class=\"w3-bar-item w3-button\">Codes</a>\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button\" style=\"color: white; float: right;\"> Welcome to our dear user " + String(username) +  " !</a>\n";
  content += "<a href=\"/login\" class=\"w3-bar-item w3-button\" style=\"float: right;\">Login</a>\n";
  // Ajouter la balise span pour afficher l'heure
  content += "<span id=\"current-time\" class=\"w3-bar-item\" style=\"color: white; float: right; padding: 14px 16px;\">Current Time: " + timeString + "</span>\n";
  content += "</div>\n";
  content += "</div>\n";
  content += "<header class=\"w3-container w3-red w3-center\" style=\"padding:128px 16px\">\n";
  content += "<h1 class=\"w3-margin w3-jumbo\">Data types from the Airpure Station</h1>\n";
  content += "<p class=\"w3-xlarge\">Explore the data collected by the Airpure Station !</p>\n";
  content += "<ul class=\"w3-ul w3-card-4\">\n";
  content += "<br><li class=\"w3-padding-16\"> MQ-135 gas sensor :</li>\n\n";
  content += "<li class=\"w3-padding-16\"> \nThe MQ-135 sensor delivers an estimation of the quantity of carbon dioxide (in part per million). </li>\n\n";
  content += "<li class=\"w3-padding-16\"> \n";
  content += "Indeed, the MQ135 is a gas sensor commonly used to detect a variety of gases including CO₂, including ammonia, nitrogen oxides (NOx), benzene, and other volatile organic compounds (VOCs) in the air. Here's a brief explanation of how the MQ135 sensor works:<br>\n";
  content += "<ul>\n";
  content += "<li>Chemical Reaction : The MQ135 sensor contains a tin dioxide (SnO2) sensing element, which reacts with various gases present in the air.</li>\n";
  content += "<li>Heating Element : Inside the sensor, there's a small heater that heats the tin dioxide sensing element to a specific temperature (usually around 200-300°C). This elevated temperature enhances the sensitivity of the sensor to gases.</li>\n";
  content += "<li>Gas Detection : When the tin dioxide sensing element comes into contact with certain gases, their presence alters the conductivity of the element. This change in conductivity is proportional to the concentration of the detected gas in the air.</li>\n";
  content += "<li>Electrical Output: The MQ135 sensor measures the resistance changes in the tin dioxide sensing element caused by the presence of different gases. This change in resistance is converted into an electrical signal that can be measured by an analog-to-digital converter (ADC) or microcontroller.</li>\n";
  content += "<li>Calibration : The MQ135 sensor requires calibration to accurately measure gas concentrations. Calibration involves exposing the sensor to known concentrations of gases and adjusting the sensor's output accordingly.</li>\n";
  content += "<li>Interfacing : The output of the MQ135 sensor is typically analog, and it needs to be connected to a microcontroller (such as Arduino) to read the sensor's values. The microcontroller can then process the sensor data and take appropriate actions based on the detected gas concentrations.</li>\n";
  content += "</ul>\n";
  content += "Overall, the MQ135 sensor is a cost-effective solution for monitoring air quality and detecting the presence of various harmful gases in the environment. However, it's essential to understand its limitations and calibration requirements to ensure accurate gas measurements. \n";
  content += "</li>\n";
  content += "<li class=\"w3-padding-16\"> Detailed Carbon Dioxide Levels Chart :</li>\n\n";
  content += "<br><table border=\"1\">\n";
  content += "<tr><th>Carbon Dioxide Levels</th><th>PPM</th><th>Percentage</th><th>Description</th></tr>\n";
  content += "<tr><td>400ppm</td><td>400</td><td>0.04%</td><td>Normal outdoor air</td></tr>\n";
  content += "<tr><td>400-1,000ppm</td><td>400-1,000</td><td>0.04-0.1%</td><td>Typical CO2 levels found indoors</td></tr>\n";
  content += "<tr><td>1,000-2,000ppm</td><td>1,000-2,000</td><td>0.1-0.2%</td><td>Common complaints of drowsiness or poor air quality</td></tr>\n";
  content += "<tr><td>2,000-5,000ppm</td><td>2,000-5,000</td><td>0.2-0.5%</td><td>Headaches, fatigue, stagnant, stuffiness, poor concentration, loss of focus, increased heart rate, nausea</td></tr>\n";
  content += "<tr><td>&gt; 50,000ppm</td><td>&gt; 50,000</td><td>&gt; 5%</td><td>Toxicity due to oxygen deprivation occurs</td></tr>\n";
  content += "<tr><td>&gt; 100,000ppm</td><td>&gt; 100,000</td><td>&gt; 10%</td><td>Oxygen deprivation in seconds: convulsions, coma, and death</td></tr>\n";
  content += "</table>\n\n";
  content += "<br><li class=\"w3-padding-16\"> SDS-011 dust particles sensor :</li>\n\n";
  content += "<li class=\"w3-padding-16\"> \n";
  content += "The SDS011 is a particulate matter (PM) sensor commonly used to detect fine dust particles (PM2.5 and PM10) in the air. Here's a brief explanation of how the SDS011 sensor works:<br>\n";
  content += "<ul>\n";
  content += "<li>Laser Scattering : The SDS011 sensor uses laser scattering technology to detect particulate matter in the air.</li>\n";
  content += "<li>Light Source : Inside the sensor, there's a laser diode that emits light into the air.</li>\n";
  content += "<li>Particle Detection : When fine dust particles pass through the laser beam, they scatter light, which is then detected by a photodetector.</li>\n";
  content += "<li>Measurement : Based on the intensity of the scattered light, the SDS011 sensor calculates the concentration of PM2.5 and PM10 particles in the air.</li>\n";
  content += "<li>Data Output : The SDS011 sensor provides real-time measurements of PM2.5 and PM10 concentrations, typically through a serial interface.</li>\n";
  content += "<li>Interfacing : The output of the SDS011 sensor can be connected to a microcontroller (such as Arduino) to read the sensor's values. The microcontroller can then process the sensor data and take appropriate actions based on the detected particulate matter concentrations.</li>\n";
  content += "</ul>\n";
  content += "Overall, the SDS011 sensor is an effective tool for monitoring air quality and assessing the level of fine dust particles present in the environment. It plays a crucial role in detecting pollution and promoting public health awareness.\n";
  content += "</li>\n";
  content += "<br><table border=\"1\">\n";
  content += "<tr><th>Particulate Matter Levels</th><th>PM2.5 (µg/m³)</th><th>PM10 (µg/m³)</th><th>Description</th></tr>\n";
  content += "<tr><td>Low</td><td>0-12</td><td>0-55</td><td>Good air quality</td></tr>\n";
  content += "<tr><td>Moderate</td><td>12-35.4</td><td>55-150.4</td><td>Air quality is acceptable; however, there may be some pollution present</td></tr>\n";
  content += "<tr><td>High</td><td>35.5-55.4</td><td>150.5-250.4</td><td>Poor air quality; sensitive groups may experience health effects</td></tr>\n";
  content += "<tr><td>Very High</td><td>55.5-150.4</td><td>250.5-350.4</td><td>Hazardous air quality; everyone may experience health effects</td></tr>\n";
  content += "<tr><td>Hazardous</td><td>150.5+</td><td>350.5+</td><td>Very unhealthy air quality; health warnings of emergency conditions</td></tr>\n";
  content += "</table>\n\n";
  content += "<br><li class=\"w3-padding-16\"> LDR photoresistor :</li>\n\n";content += "<li class=\"w3-padding-16\"> \n";
  content += "The LDR (Light Dependent Resistor) photoresistor sensor is commonly used to detect ambient light levels. Here's a brief explanation of how the LDR sensor works:<br>\n";
  content += "<ul>\n";
  content += "<li>Photoconductivity : The LDR sensor's resistance changes in response to the intensity of light falling on its surface.</li>\n";
  content += "<li>Light Sensing : When exposed to light, the LDR's resistance decreases, allowing more current to flow through it.</li>\n";
  content += "<li>Dark Resistance : In low-light conditions or darkness, the LDR's resistance increases, reducing the flow of current through the sensor.</li>\n";
  content += "<li>Resistance Range : The resistance of the LDR typically ranges from several hundred ohms in bright light to several megaohms in darkness.</li>\n";
  content += "<li>Analog Output : The LDR sensor provides an analog output that can be read by microcontrollers, allowing the measurement of ambient light levels.</li>\n";
  content += "<li>Applications : LDR sensors are commonly used in automatic lighting systems, daylight detection circuits, and outdoor light intensity monitoring.</li>\n";
  content += "</ul>\n";
  content += "Overall, the LDR photoresistor sensor is a simple yet effective component for detecting and responding to changes in ambient light levels, enabling various applications in lighting control and environmental sensing.\n";
  content += "</li>\n";
  content += "<br><table border=\"1\">\n";
  content += "<tr><th>Luminosité (lux)</th><th>Description</th></tr>\n";
  content += "<tr><td>0.002</td><td>Nuit par temps clair, sans lune.</td></tr>\n";
  content += "<tr><td>0.2</td><td>Minimum de lumière que doit produire un éclairage d’urgence.</td></tr>\n";
  content += "<tr><td>0.27 - 1</td><td>Pleine lune par temps clair.</td></tr>\n";
  content += "<tr><td>3.4</td><td>Limite crépusculaire (sombre) au couché du soleil en zone urbaine.</td></tr>\n";
  content += "<tr><td>50</td><td>Éclairage d’un salon.</td></tr>\n";
  content += "<tr><td>80</td><td>Éclairage des toilettes.</td></tr>\n";
  content += "<tr><td>100</td><td>Journée très sombre/temps très couvert.</td></tr>\n";
  content += "<tr><td>300 - 500</td><td>Levé du soleil, luminosité par temps clair. Zone de bureau correctement éclairée.</td></tr>\n"; 
  content += "<tr><td>1,000</td><td>Temps couvert; Éclairage typique d’un studio TV.</td></tr>\n";
  content += "<tr><td>10,000 - 25,000</td><td>Pleine journée (pas de soleil direct).</td></tr>\n";
  content += "<tr><td>32,000 - 130,000</td><td>Soleil direct.</td></tr>\n";
  content += "</table>\n\n";
  content += "<br><table border=\"1\">\n";
  content += "<tr><th>Light Levels</th><th>Description</th></tr>\n";
  content += "<tr><td>High</td><td>Strong ambient light conditions</td></tr>\n";
  content += "<tr><td>Normal</td><td>Typical indoor lighting levels</td></tr>\n";
  content += "<tr><td>Low</td><td>Dimly lit or low-light environments</td></tr>\n";
  content += "<tr><td>Dark</td><td>Very low or no ambient light present</td></tr>\n";
  content += "</table>\n\n";
  content += "<br><li class=\"w3-padding-16\">BME-280 temperature, humidity, pressure and altitude</li>\n\n";
  content += "<li class=\"w3-padding-16\"> \n";
  content += "The BME280 is a versatile environmental sensor commonly used to measure temperature, humidity, and barometric pressure in the air. Here's a brief explanation of how the BME280 sensor works:<br>\n";
  content += "<ul>\n";
  content += "<li>Temperature Sensing : The BME280 sensor contains a built-in temperature sensor that accurately measures ambient temperature.</li>\n";
  content += "<li>Humidity Sensing : With its integrated humidity sensor, the BME280 can measure the relative humidity of the surrounding air.</li>\n";
  content += "<li>Pressure Sensing : The BME280 sensor also features a barometric pressure sensor capable of measuring atmospheric pressure.</li>\n";
  content += "<li>Sensor Fusion : The BME280 employs sensor fusion algorithms to combine data from its temperature, humidity, and pressure sensors, providing accurate environmental readings.</li>\n";
  content += "<li>Communication Interface : The BME280 sensor communicates with microcontrollers (such as Arduino) via standard communication protocols like I2C or SPI.</li>\n";
  content += "<li>Data Processing : Once connected to a microcontroller, the BME280 sensor provides raw sensor data that can be processed and analyzed to monitor weather conditions and environmental changes.</li>\n";
  content += "</ul>\n";
  content += "Overall, the BME280 sensor is a reliable and cost-effective solution for various applications requiring environmental monitoring, weather forecasting, and indoor air quality assessment.\n";
  content += "</li>\n";
  content += "<br><table border=\"1\">\n";
  content += "<tr><th>Environmental Conditions</th><th>Temperature (°C)</th><th>Humidity (%)</th><th>Pressure (hPa)</th><th>Description</th></tr>\n";
  content += "<tr><td>Normal</td><td>20-25</td><td>30-50</td><td>1013.25</td><td>Standard environmental conditions</td></tr>\n";
  content += "<tr><td>Warm</td><td>25-30</td><td>50-70</td><td>1013.25</td><td>Elevated temperature and humidity levels</td></tr>\n";
  content += "<tr><td>Cool</td><td>15-20</td><td>30-50</td><td>1013.25</td><td>Lower temperature with standard humidity</td></tr>\n";
  content += "<tr><td>Dry</td><td>20-25</td><td>20-30</td><td>1013.25</td><td>Lower humidity levels with standard temperature</td></tr>\n";
  content += "<tr><td>High Pressure</td><td>20-25</td><td>30-50</td><td>1013.25+</td><td>Elevated atmospheric pressure conditions</td></tr>\n";
  content += "</table>\n\n";
  content += "</ul>\n";
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
  // Lire l'heure actuelle du module RTC
  DateTime now = rtc.now();

  // Construire la chaîne représentant l'heure actuelle avec les secondes
  String timeString = String(now.hour()) + ":" + 
                      String(now.minute()) + ":" + 
                      String(now.second());

  // Construire la chaîne représentant la date actuelle
  String dateString = String(now.day()) + "/" + 
                      String(now.month()) + "/" + 
                      String(now.year());

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
  content += "<script>\n";
  content += "function showMoreInfo() {\n";
  content += "  var x = document.getElementById('moreInfo');\n";
  content += "  if (x.style.display === 'none') {\n";
  content += "    x.style.display = 'block';\n";
  content += "  } else {\n";
  content += "    x.style.display = 'none';\n";
  content += "  }\n";
  content += "}\n";
  content += "function updateTime() {\n";
  content += "  var currentTime = new Date();\n";
  content += "  var hours = currentTime.getHours();\n";
  content += "  var minutes = currentTime.getMinutes();\n";
  content += "  var seconds = currentTime.getSeconds();\n";
  content += "  var timeString = hours + ':' + (minutes < 10 ? '0' : '') + minutes + ':' + (seconds < 10 ? '0' : '') + seconds;\n";
  content += "  document.getElementById('current-time').innerText = timeString;\n";
  content += "  setTimeout(updateTime, 1000); // Mettre à jour chaque seconde\n";
  content += "}\n";
  content += "window.onload = function() {\n";
  content += "  updateTime();\n";
  content += "}\n";
  content += "</script>\n";
  content += "</head><body>\n";
  content += "<div class=\"w3-top\">\n";
  content += "<div class=\"w3-bar w3-card\">\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button\">The Airpure Project</a>\n";
  content += "<a href=\"/weather\" class=\"w3-bar-item w3-button\">The Airpure Weather Station</a>\n";
  content += "<a href=\"/data\" class=\"w3-bar-item w3-button\">Data</a>\n";
  content += "<a href=\"/specifications\" class=\"w3-bar-item w3-button active\">Specifications</a>\n";
  content += "<a href=\"/codes\" class=\"w3-bar-item w3-button\">Codes</a>\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button\" style=\"color: white; float: right;\"> Welcome to our dear user " + String(username) +  " !</a>\n";
  content += "<a href=\"/login\" class=\"w3-bar-item w3-button\" style=\"float: right;\">Login</a>\n";
  // Ajouter la balise span pour afficher l'heure
  content += "<span id=\"current-time\" class=\"w3-bar-item\" style=\"color: white; float: right; padding: 14px 16px;\">Current Time: " + timeString + "</span>\n";
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
  content += "<li class=\"w3-padding-16\"> User-friendly web interface for data visualization and analysis</li>\n\n";
  content += "</ul></div></div></div></div></body></html>\n";
  String footer = "<footer style=\"background-color: #1e3d59; padding: 20px; text-align: center;\">"
                  "<p><span style=\"color: white;\">Visit my Github page for more informations :</span></p>"
                  "<a href=\"https://github.com/juldebidour/P2I\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Link to my Github page</button></a>"
                  "<p><span style=\"color: white;\">Visit the ENSC website for more informations :</span></p>"
                  "<a href=\"https://ensc.bordeaux-inp.fr/fr\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Link to the ENSC Website</button></a>"
                  "</footer>\n";

  content += footer;
  content += "</body>\n";
  content += "</html>\n";
  server.send(200, "text/html", content);
}

void handleCodes() {
  // Lire l'heure actuelle du module RTC
  DateTime now = rtc.now();

  // Construire la chaîne représentant l'heure actuelle avec les secondes
  String timeString = String(now.hour()) + ":" + 
                      String(now.minute()) + ":" + 
                      String(now.second());

  // Construire la chaîne représentant la date actuelle
  String dateString = String(now.day()) + "/" + 
                      String(now.month()) + "/" + 
                      String(now.year());

  String content = "<html>\n";
  content += "<head>\n";
  content += "<title>Codes of the Airpure Station</title>\n";
  content += "<meta charset=\"UTF-8\">\n";
  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">\n";
  content += "<style>\n";
  content += "body {\n";
  content += "  font-family: Arial, sans-serif;\n";
  content += "  margin: 0;\n";
  content += "  padding: 0;\n";
  content += "}\n";
  content += ".w3-top {\n";
  content += "  position: sticky;\n";
  content += "  top: 0;\n";
  content += "  z-index: 9999;\n";
  content += "}\n";
  content += ".w3-bar {\n";
  content += "  overflow: hidden;\n";
  content += "  background-color: #1e3d59;\n";
  content += "}\n";
  content += ".w3-bar a {\n";
  content += "  float: left;\n";
  content += "  display: block;\n";
  content += "  color: white;\n";
  content += "  text-align: center;\n";
  content += "  padding: 14px 16px;\n";
  content += "  text-decoration: none;\n";
  content += "}\n";
  content += ".w3-bar a:hover {\n";
  content += "  background-color: #ddd;\n";
  content += "  color: black;\n";
  content += "}\n";
  content += ".w3-bar a.active {\n";
  content += "  background-color: #f5f0e1;\n";
  content += "  color: black;\n";
  content += "}\n";
  content += ".code-container {\n";
  content += "  display: none;\n";
  content += "}\n";
  content += "</style>\n";
  content += "<script>\n";
  content += "function toggleCode() {\n";
  content += "  var x = document.getElementById('code-container');\n";
  content += "  if (x.style.display === 'none') {\n";
  content += "    x.style.display = 'block';\n";
  content += "  } else {\n";
  content += "    x.style.display = 'none';\n";
  content += "  }\n";
  content += "}\n";
  content += "function showMoreInfo() {\n";
  content += "  var x = document.getElementById('moreInfo');\n";
  content += "  if (x.style.display === 'none') {\n";
  content += "    x.style.display = 'block';\n";
  content += "  } else {\n";
  content += "    x.style.display = 'none';\n";
  content += "  }\n";
  content += "}\n";
  content += "function updateTime() {\n";
  content += "  var currentTime = new Date();\n";
  content += "  var hours = currentTime.getHours();\n";
  content += "  var minutes = currentTime.getMinutes();\n";
  content += "  var seconds = currentTime.getSeconds();\n";
  content += "  var timeString = hours + ':' + (minutes < 10 ? '0' : '') + minutes + ':' + (seconds < 10 ? '0' : '') + seconds;\n";
  content += "  document.getElementById('current-time').innerText = timeString;\n";
  content += "  setTimeout(updateTime, 1000); // Mettre à jour chaque seconde\n";
  content += "}\n";
  content += "window.onload = function() {\n";
  content += "  updateTime();\n";
  content += "}\n";
  content += "</script>\n";
  content += "</head>\n";
  content += "<body>\n";
  content += "<div class=\"w3-top\">\n";
  content += "<div class=\"w3-bar w3-card\">\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button\">The Airpure Project</a>\n";
  content += "<a href=\"/weather\" class=\"w3-bar-item w3-button\">The Airpure Weather Station</a>\n";
  content += "<a href=\"/data\" class=\"w3-bar-item w3-button\">Data</a>\n";
  content += "<a href=\"/specifications\" class=\"w3-bar-item w3-button\">Specifications</a>\n";
  content += "<a href=\"/codes\" class=\"w3-bar-item w3-button active\" onclick=\"toggleCode()\">Codes</a>\n";
  content += "<a href=\"/\" class=\"w3-bar-item w3-button\" style=\"color: white; float: right;\"> Welcome to our dear user " + String(username) +  " !</a>\n";
  content += "<a href=\"/login\" class=\"w3-bar-item w3-button\" style=\"float: right;\">Login</a>\n";
  // Ajouter la balise span pour afficher l'heure
  content += "<span id=\"current-time\" class=\"w3-bar-item\" style=\"color: white; float: right; padding: 14px 16px;\">Current Time: " + timeString + "</span>\n";
  content += "</div>\n";
  content += "</div>\n";
  content += "<header class=\"w3-container w3-red w3-center\" style=\"padding:32px 16px\">\n";
  content += "<h1 class=\"w3-margin w3-xxlarge\">Codes of the Airpure Station</h1>\n";
  content += "<p class=\"w3-large\">Explore the codes behind the Airpure Station !</p>\n";
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
  // Insert the full code here
  content += "<pre><code>";
  // Insert the full code here
  content += "</code></pre>";
  content += "</ul></div></div></div></div></body></html>\n";
  String footer = "<footer style=\"background-color: #1e3d59; padding: 20px; text-align: center;\">"
                  "<p><span style=\"color: white;\">Visit my Github page for more informations :</span></p>"
                  "<a href=\"https://github.com/juldebidour/P2I\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Link to my Github page</button></a>"
                  "<p><span style=\"color: white;\">Visit the ENSC website for more informations :</span></p>"
                  "<a href=\"https://ensc.bordeaux-inp.fr/fr\"><button style=\"padding: 10px 20px; background-color: #f5f0e1; color: black; border: none; cursor: pointer; border-radius: 5px;\">Link to the ENSC Website</button></a>"
                  "</footer>\n";

  content += footer;
  content += "</body>\n";
  content += "</html>\n";

  server.send(200, "text/html", content);
}

void setup() {
  Serial.begin(115200);

  // Connecter au WiFi
  WiFi.softAP(ssid, password);
  Serial.println("WiFi AP Started");

  sds011.begin(SDS_PIN_RX, SDS_PIN_TX);

  pinMode(Pin_D1,OUTPUT);
  pinMode(Pin_D2,OUTPUT);

  // Initialize BME280 sensor
  if (!bme.begin(0x76)) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");

  // Initialisation du module RTC DS3231
    if (!rtc.begin()) {
        Serial.println("Couldn't find RTC");
    }
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Définir les routes du serveur
  server.on("/", HTTP_GET, handleRoot);
  server.on("/weather", HTTP_GET, handleWeather);
  server.on("/data", HTTP_GET, handleData);
  server.on("/specifications", HTTP_GET, handleSpecifications);
  server.on("/codes", HTTP_GET, handleCodes);
  server.on("/login", HTTP_GET, handleLogin);

  // Démarrer le serveur
  server.begin();
  Serial.println("HTTP server started");
}

int analogRead1() {
    digitalWrite(Pin_D1, HIGH); // Turn D1 On
    digitalWrite(Pin_D2, LOW); // Turn D2 Off
    return analogRead(0);
}
 
int analogRead2() {
    digitalWrite(Pin_D1, LOW); //  Turn D1 On
    digitalWrite(Pin_D2, HIGH); // Turn D2 Off
    return analogRead(0);
}

void loop() {
  // Gérer les requêtes client
  server.handleClient();
}