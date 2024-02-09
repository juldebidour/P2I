#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>

const char* ssid = "AutoConnectAP";
const char* password = "password";

ESP8266WebServer server(80);

void handleRoot() {
  String content = "<html><head><title>Bienvenue sur AIRPURE_STATION</title>";
  content += "<meta charset=\"UTF-8\">";
  content += "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">";
  content += "<link rel=\"stylesheet\" href=\"https://www.w3schools.com/w3css/4/w3.css\">";
  content += "<link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/css?family=Lato\">";
  content += "<link rel=\"stylesheet\" href=\"https://fonts.googleapis.com/css?family=Montserrat\">";
  content += "<link rel=\"stylesheet\" href=\"https://cdnjs.cloudflare.com/ajax/libs/font-awesome/4.7.0/css/font-awesome.min.css\">";
  content += "</head><body>";
  content += "<div class=\"w3-top\">";
  content += "<div class=\"w3-bar w3-red w3-card w3-left-align w3-large\">";
  content += "<a class=\"w3-bar-item w3-button w3-hide-medium w3-hide-large w3-right w3-padding-large w3-hover-white w3-large w3-red\" href=\"javascript:void(0);\" onclick=\"myFunction()\" title=\"Toggle Navigation Menu\"><i class=\"fa fa-bars\"></i></a>";
  content += "<a href=\"#\" class=\"w3-bar-item w3-button w3-padding-large w3-white\">The Airpure Project</a>";
  content += "<a href=\"#\" class=\"w3-bar-item w3-button w3-hide-small w3-padding-large w3-hover-white\">The Airpure Weather Station</a>";
  content += "<a href=\"#\" class=\"w3-bar-item w3-button w3-hide-small w3-padding-large w3-hover-white\">Data</a>";
  content += "<a href=\"#\" class=\"w3-bar-item w3-button w3-hide-small w3-padding-large w3-hover-white\">Specifications</a>";
  content += "<a href=\"#\" class=\"w3-bar-item w3-button w3-hide-small w3-padding-large w3-hover-white\">Codes</a>";
  content += "</div>";
  content += "<div id=\"navDemo\" class=\"w3-bar-block w3-white w3-hide w3-hide-large w3-hide-medium w3-large\">";
  content += "<a href=\"#\" class=\"w3-bar-item w3-button w3-padding-large\">The Airpure Weather Station</a>";
  content += "<a href=\"#\" class=\"w3-bar-item w3-button w3-padding-large\">Data</a>";
  content += "<a href=\"#\" class=\"w3-bar-item w3-button w3-padding-large\">Specifications</a>";
  content += "<a href=\"#\" class=\"w3-bar-item w3-button w3-padding-large\">Codes</a>";
  content += "</div></div>";
  content += "<header class=\"w3-container w3-red w3-center\" style=\"padding:128px 16px\">";
  content += "<h1 class=\"w3-margin w3-jumbo\">Welcome to the Airpure Station Project !</h1>";
  content += "<p class=\"w3-xlarge\">a personal computing project by Julien Debidour/p>";
  content += "<button class=\"w3-button w3-black w3-padding-large w3-large w3-margin-top\">See more informations !</button>";
  content += "</header>";
  content += "<div class=\"w3-row-padding w3-padding-64 w3-container\">";
  content += "<div class=\"w3-content\"><div class=\"w3-twothird\"><h1>The Airpure Station Project in brief :</h1>";
  content += "<h5 class=\"w3-padding-32\">This project aims to set up a connected station using gas sensors such as SDS 011, MQ135 or BMP 280, in particular to monitor air quality. It will allow access to data collected remotely by a remote server. This is a personal initiative that addresses a growing need for air quality monitoring. <br> A stand-alone weather and air composition station will be set up to monitor these different variables. <br> The aim to deliver are composed of a connected station integrating SDS 011 and MQ135 sensors, a web user/remote server interface to visualize the collected data, an a data storage system.</p>";
  content += "</div><div class=\"w3-third w3-center\"><i class=\"fa fa-anchor w3-padding-64 w3-text-red\"></i></div>";
  content += "</div></div><div class=\"w3-row-padding w3-light-grey w3-padding-64 w3-container\">";
  content += "<div class=\"w3-content\"><div class=\"w3-third w3-center\"><i class=\"fa fa-coffee w3-padding-64 w3-text-red w3-margin-right\"></i></div>";
  content += "<div class=\"w3-twothird\"><h1>Use cases and context :</h1><h5 class=\"w3-padding-32\">The station will be used in specific areas to monitor air quality. End users will be able to access the data via a secure web interface from anywhere, at any time. <br> It will collect and store air quality data, including :<ul><li><br> Measurements of SDS011 and MQ135 sensors. </li><br> <li>Data on air pollution.</li> <br> <li>Variations in detected gas concentrations.</li></p>";
  content += "</div></div></div><div class=\"w3-container w3-black w3-center w3-opacity w3-padding-64\">";
  content += "<h1 class=\"w3-margin w3-xlarge\">Quote of the day: Be safe, breathe better !</h1></div>";
  content += "<footer class=\"w3-container w3-padding-64 w3-center w3-opacity\">";
  content += "<div class=\"w3-xlarge w3-padding-32\"><i class=\"fa fa-facebook-official w3-hover-opacity\"></i>";
  content += "<i class=\"fa fa-instagram w3-hover-opacity\"></i><i class=\"fa fa-snapchat w3-hover-opacity\"></i>";
  content += "<i class=\"fa fa-pinterest-p w3-hover-opacity\"></i><i class=\"fa fa-twitter w3-hover-opacity\"></i>";
  content += "<i class=\"fa fa-linkedin w3-hover-opacity\"></i></div>";
  content += "<p> Thanks for reading it ! I hope you liked it !</p>";
  content += "</footer><script>function myFunction() { var x = document.getElementById(\"navDemo\");";
  content += "if (x.className.indexOf(\"w3-show\") == -1) { x.className += \" w3-show\"; } else { x.className = x.className.replace(\" w3-show\", \"\"); }}</script></body></html>";

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
