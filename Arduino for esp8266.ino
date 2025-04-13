#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

// Servo declarations
Servo base;
Servo elbow;
Servo joint;
Servo wrist;
Servo grip;

// Define WiFi credentials
const char *ssid = "ARM";
const char *password = "12345678";

// Create web server instance on port 80
ESP8266WebServer server(80);

// Store servo positions (initialized at center 90 degrees)
int basePos = 90, elbowPos = 90, jointPos = 90, wristPos = 90, gripPos = 90;

void setup() {
  Serial.begin(115200);
  
  base.attach(D0);
  elbow.attach(D1);
  joint.attach(D2);
  wrist.attach(D3);
  grip.attach(D4);

  WiFi.softAP(ssid, password);
  Serial.println("Access Point Started");
  Serial.print("IP Address: ");
  Serial.println(WiFi.softAPIP());

  // Define web server routes
  server.on("/", HTTP_GET, handleRoot);
  server.on("/move", HTTP_GET, moveServos);

  server.begin();
  Serial.println("Server started");
  resetServos();
}

// Function to reset servos to 90 degrees
void resetServos() {
  base.write(basePos);
  elbow.write(elbowPos);
  joint.write(jointPos);
  wrist.write(wristPos);
  grip.write(gripPos);
}

// Function to move servos based on received values
void moveServos() {
  if (server.hasArg("base")) basePos = constrain(server.arg("base").toInt(), 0, 180);
  if (server.hasArg("elbow")) elbowPos = constrain(server.arg("elbow").toInt(), 0, 180);
  if (server.hasArg("joint")) jointPos = constrain(server.arg("joint").toInt(), 0, 180);
  if (server.hasArg("wrist")) wristPos = constrain(server.arg("wrist").toInt(), 0, 180);
  if (server.hasArg("grip")) gripPos = constrain(server.arg("grip").toInt(), 0, 180);
  
  base.write(basePos);
  elbow.write(elbowPos);
  joint.write(jointPos);
  wrist.write(wristPos);
  grip.write(gripPos);
  
  server.send(200, "text/plain", "Servos moved");
}

// Webpage with joystick controls
void handleRoot() {
  String html = "<html><body>";
  html += "<h2>Joystick Control Panel</h2>";
  html += "<script>";
  html += "function update() {";
  html += "  let base = document.getElementById('base').value;";
  html += "  let elbow = document.getElementById('elbow').value;";
  html += "  let joint = document.getElementById('joint').value;";
  html += "  let wrist = document.getElementById('wrist').value;";
  html += "  let grip = document.getElementById('grip').value;";
  html += "  let xhr = new XMLHttpRequest();";
  html += "  xhr.open('GET', '/move?base=' + base + '&elbow=' + elbow + '&joint=' + joint + '&wrist=' + wrist + '&grip=' + grip, true);";
  html += "  xhr.send();";
  html += "}";
  html += "</script>";
  html += "Base: <input type='range' id='base' min='0' max='180' value='90' oninput='update()'><br><br>";
  html += "Elbow: <input type='range' id='elbow' min='0' max='180' value='90' oninput='update()'><br><br>";
  html += "Joint: <input type='range' id='joint' min='0' max='180' value='90' oninput='update()'><br><br>";
  html += "Wrist: <input type='range' id='wrist' min='0' max='180' value='90' oninput='update()'><br><br>";
  html += "Grip: <input type='range' id='grip' min='0' max='180' value='90' oninput='update()'><br><br>";
  html += "</body></html>";

  server.send(200, "text/html", html);
}
void loop() {
  server.handleClient();
}