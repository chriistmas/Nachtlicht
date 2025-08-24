/*
You can Upload the Directly. Don't Need Change Anything.

Used Libraries:

AsyncTCP: https://github.com/dvarrel/AsyncTCP
ESPAsyncWebServer: https://github.com/lacamera/ESPAsyncWebServer
*/

// Include necessary libraries
#include <Arduino.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <iostream>
#include <sstream>

// Define a struct to hold motor control pin assignments
struct MOTOR_PINS {
  int pinEn;   // Enable pin (PWM)
  int pinIN1;  // Direction control pin 1
  int pinIN2;  // Direction control pin 2
};

// Create a vector holding pin configurations for both motors
// Updated to match your working test1.ino configuration
std::vector<MOTOR_PINS> motorPins = {
  {23, 19, 18},  // RIGHT_MOTOR Pins (enB, IN3, IN4) - Physical right motor
  {5, 22, 21},   // LEFT_MOTOR  Pins (enA, IN1, IN2) - Physical left motor
};

// Define WiFi AP credentials
const char* ssid     = "Mini_Soccer_Bot";  // WiFi SSID
const char* password = "12345678";         // WiFi Password

// Define command constants for directions
#define UP 1
#define DOWN 2
#define LEFT 3
#define RIGHT 4
#define STOP 0

#define FORWARD_LEFT 5
#define FORWARD_RIGHT 6
#define BACKWARD_LEFT 7
#define BACKWARD_RIGHT 8

// Define motor indexes
#define RIGHT_MOTOR 0
#define LEFT_MOTOR 1

// Define motor directions
#define FORWARD 1
#define BACKWARD -1

// PWM configuration - Updated for ESP32 2.0.11 compatibility
const int PWMFreq = 5000;            // PWM frequency in Hz (matches your test1.ino)
const int PWMResolution = 8;         // 8-bit resolution (0-255)
const int PWMSpeedChannelA = 0;      // PWM channel for left motor (enA)
const int PWMSpeedChannelB = 1;      // PWM channel for right motor (enB)

// Create AsyncWebServer on port 80 and WebSocket endpoint
AsyncWebServer server(80);
AsyncWebSocket wsCarInput("/CarInput");

// HTML page served to clients
const char* htmlHomePage PROGMEM = R"HTML(
<!DOCTYPE html>
<html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1, maximum-scale=1, user-scalable=no">
  <style>
    /* Styling the control UI */
    body {
      background-color: #fdfd96;
      font-family: Arial, sans-serif;
      margin: 0;
      padding: 10px;
      overflow-x: auto;
      display: flex;
      flex-direction: column;
      align-items: center;
      min-height: 100vh;
    }
    .main-container {
      display: flex;
      flex-direction: row;
      gap: 20px;
      max-width: 1200px;
      width: 100%;
      justify-content: center;
      align-items: flex-start;
    }
    .controls-section {
      display: flex;
      flex-direction: row;
      justify-content: space-between;
      min-width: 400px;
      align-items: center;
    }
    .map-section {
      display: flex;
      flex-direction: column;
      align-items: center;
      gap: 10px;
    }
    .column {
      display: flex;
      flex-direction: column;
      gap: 20px;
    }
    .right-controls {
      display: flex;
      flex-direction: column;
      align-items: center;
      gap: 20px;
    }
    .row {
      display: flex;
      flex-direction: row;
      gap: 20px;
    }
    .button {
      width: 80px;
      height: 80px;
      background-color: white;
      border-radius: 10px;
      box-shadow: 5px 5px #888888;
      display: flex;
      justify-content: center;
      align-items: center;
      user-select: none;
      font-size: 12px;
      font-weight: bold;
    }
    .button:active {
      transform: translate(5px, 5px);
      box-shadow: none;
    }
    .slider-container {
      width: 100%;
      text-align: center;
    }
    .slider {
      width: 100%;
    }
    #mapCanvas {
      border: 2px solid #333;
      background-color: #e8f5e8;
      border-radius: 10px;
    }
    .map-controls {
      display: flex;
      gap: 10px;
      flex-wrap: wrap;
      justify-content: center;
    }
    .map-button {
      padding: 8px 16px;
      background-color: white;
      border: 1px solid #ccc;
      border-radius: 5px;
      cursor: pointer;
      font-size: 12px;
    }
    .map-button:hover {
      background-color: #f0f0f0;
    }
    .status-info {
      background-color: white;
      padding: 10px;
      border-radius: 5px;
      border: 1px solid #ccc;
      font-size: 12px;
      text-align: center;
      max-width: 300px;
    }
    @media (max-width: 768px) {
      .main-container {
        flex-direction: column;
        align-items: center;
      }
      .controls-section {
        min-width: auto;
        width: 100%;
        max-width: 400px;
      }
    }
  </style>
</head>
<body>
  <div class="main-container">
    <!-- Controls Section -->
    <div class="controls-section">
      <div class="column">
        <div class="button" ontouchstart='sendMove("1")' ontouchend='sendMove("0")' onmousedown='sendMove("1")' onmouseup='sendMove("0")'>UP</div>
        <div class="button" ontouchstart='sendMove("2")' ontouchend='sendMove("0")' onmousedown='sendMove("2")' onmouseup='sendMove("0")'>DOWN</div>
      </div>
      <div class="right-controls">
        <div class="slider-container">
          <label for="Speed">Speed:</label><br>
          <input type="range" min="0" max="255" value="150" class="slider" id="Speed" oninput='updateSpeed(value)'>
        </div>
        <div class="row">
          <div class="button" ontouchstart='sendMove("3")' ontouchend='sendMove("0")' onmousedown='sendMove("3")' onmouseup='sendMove("0")'>LEFT</div>
          <div class="button" ontouchstart='sendMove("4")' ontouchend='sendMove("0")' onmousedown='sendMove("4")' onmouseup='sendMove("0")'>RIGHT</div>
        </div>
      </div>
    </div>
    
    <!-- Map Section -->
    <div class="map-section">
      <h3 style="margin: 0;">Car Navigation Map</h3>
      <canvas id="mapCanvas" width="400" height="300"></canvas>
      <div class="map-controls">
        <button class="map-button" onclick="clearPath()">Clear Path</button>
        <button class="map-button" onclick="resetPosition()">Reset Position</button>
        <button class="map-button" onclick="toggleGrid()">Toggle Grid</button>
      </div>
      <div class="status-info">
        <div>Position: <span id="position">X: 200, Y: 150</span></div>
        <div>Direction: <span id="direction">North</span></div>
        <div>Speed: <span id="currentSpeed">150</span></div>
        <div>Status: <span id="status">Stopped</span></div>
      </div>
    </div>
  </div>

  <script>
    var socket;
    var mapCanvas, ctx;
    var carPosition = { x: 200, y: 150 }; // Center of canvas
    var carDirection = 0; // 0=North, 90=East, 180=South, 270=West
    var pathHistory = [];
    var currentSpeed = 150;
    var isMoving = false;
    var showGrid = true;
    var movementInterval;
    
    // Direction mappings
    const directions = ['North', 'East', 'South', 'West'];
    
    // Establish WebSocket connection
    function connect() {
      socket = new WebSocket("ws://" + location.host + "/CarInput");
      socket.onopen = () => {
        send("Speed", document.getElementById("Speed").value); // Set initial speed
        updateDisplay();
      };
      socket.onclose = () => setTimeout(connect, 2000); // Reconnect if disconnected
    }

    // Send key-value pair through WebSocket
    function send(key, value) {
      if (socket && socket.readyState === WebSocket.OPEN) {
        socket.send(key + "," + value);
      }
    }
    
    // Enhanced movement function with map integration
    function sendMove(direction) {
      send("MoveCar", direction);
      
      if (direction === "0") {
        // Stop movement
        isMoving = false;
        if (movementInterval) {
          clearInterval(movementInterval);
          movementInterval = null;
        }
        updateStatus("Stopped");
      } else {
        // Start movement
        isMoving = true;
        updateCarDirection(direction);
        updateStatus("Moving " + directions[Math.floor(carDirection / 90)]);
        
        // Start movement simulation on map
        if (movementInterval) clearInterval(movementInterval);
        movementInterval = setInterval(() => {
          if (isMoving) {
            simulateMovement(direction);
          }
        }, 100); // Update position every 100ms
      }
    }
    
    function updateCarDirection(direction) {
      switch(direction) {
        case "1": carDirection = 0; break;   // UP = North
        case "2": carDirection = 180; break; // DOWN = South  
        case "3": carDirection = 270; break; // LEFT = West
        case "4": carDirection = 90; break;  // RIGHT = East
      }
    }
    
    function simulateMovement(direction) {
      const speed = currentSpeed / 255; // Normalize speed
      const moveDistance = speed * 2; // Scale movement
      
      // Add current position to path history
      pathHistory.push({ x: carPosition.x, y: carPosition.y });
      if (pathHistory.length > 500) { // Limit path history
        pathHistory.shift();
      }
      
      // Calculate new position based on direction
      switch(direction) {
        case "1": // UP
          carPosition.y = Math.max(20, carPosition.y - moveDistance);
          break;
        case "2": // DOWN
          carPosition.y = Math.min(280, carPosition.y + moveDistance);
          break;
        case "3": // LEFT
          carPosition.x = Math.max(20, carPosition.x - moveDistance);
          break;
        case "4": // RIGHT
          carPosition.x = Math.min(380, carPosition.x + moveDistance);
          break;
      }
      
      drawMap();
      updateDisplay();
    }
    
    function initMap() {
      mapCanvas = document.getElementById('mapCanvas');
      ctx = mapCanvas.getContext('2d');
      drawMap();
    }
    
    function drawMap() {
      // Clear canvas
      ctx.clearRect(0, 0, mapCanvas.width, mapCanvas.height);
      
      // Draw grid if enabled
      if (showGrid) {
        drawGrid();
      }
      
      // Draw path history
      drawPath();
      
      // Draw car
      drawCar();
    }
    
    function drawGrid() {
      ctx.strokeStyle = '#ccc';
      ctx.lineWidth = 1;
      
      // Vertical lines
      for (let x = 0; x <= mapCanvas.width; x += 20) {
        ctx.beginPath();
        ctx.moveTo(x, 0);
        ctx.lineTo(x, mapCanvas.height);
        ctx.stroke();
      }
      
      // Horizontal lines
      for (let y = 0; y <= mapCanvas.height; y += 20) {
        ctx.beginPath();
        ctx.moveTo(0, y);
        ctx.lineTo(mapCanvas.width, y);
        ctx.stroke();
      }
    }
    
    function drawPath() {
      if (pathHistory.length < 2) return;
      
      ctx.strokeStyle = '#ff6b6b';
      ctx.lineWidth = 2;
      ctx.beginPath();
      
      ctx.moveTo(pathHistory[0].x, pathHistory[0].y);
      for (let i = 1; i < pathHistory.length; i++) {
        ctx.lineTo(pathHistory[i].x, pathHistory[i].y);
      }
      ctx.stroke();
    }
    
    function drawCar() {
      ctx.save();
      ctx.translate(carPosition.x, carPosition.y);
      ctx.rotate(carDirection * Math.PI / 180);
      
      // Car body
      ctx.fillStyle = '#4CAF50';
      ctx.fillRect(-8, -12, 16, 24);
      
      // Car direction indicator (front)
      ctx.fillStyle = '#2196F3';
      ctx.fillRect(-6, -12, 12, 6);
      
      // Car outline
      ctx.strokeStyle = '#333';
      ctx.lineWidth = 2;
      ctx.strokeRect(-8, -12, 16, 24);
      
      ctx.restore();
      
      // Position marker
      ctx.fillStyle = 'rgba(255, 0, 0, 0.3)';
      ctx.beginPath();
      ctx.arc(carPosition.x, carPosition.y, 15, 0, 2 * Math.PI);
      ctx.fill();
    }
    
    function updateDisplay() {
      document.getElementById('position').textContent = 
        `X: ${Math.round(carPosition.x)}, Y: ${Math.round(carPosition.y)}`;
      document.getElementById('direction').textContent = 
        directions[Math.floor(carDirection / 90)];
      document.getElementById('currentSpeed').textContent = currentSpeed;
    }
    
    function updateStatus(status) {
      document.getElementById('status').textContent = status;
    }
    
    function clearPath() {
      pathHistory = [];
      drawMap();
    }
    
    function resetPosition() {
      carPosition = { x: 200, y: 150 };
      carDirection = 0;
      pathHistory = [];
      drawMap();
      updateDisplay();
    }
    
    function toggleGrid() {
      showGrid = !showGrid;
      drawMap();
    }
    
    // Update speed tracking
    function updateSpeed(value) {
      currentSpeed = parseInt(value);
      send("Speed", value);
      updateDisplay();
    }
    
    // Initialize everything on page load
    window.onload = function() {
      connect();
      initMap();
      updateDisplay();
    };
  </script>
</body>
</html>
)HTML";

// Function to rotate a motor in specified direction
void rotateMotor(int motorNumber, int direction) {
  if (direction == FORWARD) {
    if (motorNumber == RIGHT_MOTOR) {
      // Right motor forward (matches your test: IN3=LOW, IN4=HIGH)
      digitalWrite(motorPins[motorNumber].pinIN1, LOW);
      digitalWrite(motorPins[motorNumber].pinIN2, HIGH);
    } else {
      // Left motor forward (matches your test: IN1=HIGH, IN2=LOW)
      digitalWrite(motorPins[motorNumber].pinIN1, HIGH);
      digitalWrite(motorPins[motorNumber].pinIN2, LOW);
    }
  } else if (direction == BACKWARD) {
    if (motorNumber == RIGHT_MOTOR) {
      // Right motor backward (matches your test: IN3=HIGH, IN4=LOW)
      digitalWrite(motorPins[motorNumber].pinIN1, HIGH);
      digitalWrite(motorPins[motorNumber].pinIN2, LOW);
    } else {
      // Left motor backward (matches your test: IN1=LOW, IN2=HIGH)
      digitalWrite(motorPins[motorNumber].pinIN1, LOW);
      digitalWrite(motorPins[motorNumber].pinIN2, HIGH);
    }
  } else {
    digitalWrite(motorPins[motorNumber].pinIN1, LOW);
    digitalWrite(motorPins[motorNumber].pinIN2, LOW); // Stop
  }
}

// Function to control the car's movement based on input command
void moveCar(int cmd) {
  switch (cmd) {
    case UP: rotateMotor(RIGHT_MOTOR, FORWARD); rotateMotor(LEFT_MOTOR, FORWARD); break;
    case DOWN: rotateMotor(RIGHT_MOTOR, BACKWARD); rotateMotor(LEFT_MOTOR, BACKWARD); break;
    case LEFT: rotateMotor(RIGHT_MOTOR, FORWARD); rotateMotor(LEFT_MOTOR, BACKWARD); break;
    case RIGHT: rotateMotor(RIGHT_MOTOR, BACKWARD); rotateMotor(LEFT_MOTOR, FORWARD); break;
    case FORWARD_LEFT: rotateMotor(RIGHT_MOTOR, FORWARD); rotateMotor(LEFT_MOTOR, STOP); break;
    case FORWARD_RIGHT: rotateMotor(RIGHT_MOTOR, STOP); rotateMotor(LEFT_MOTOR, FORWARD); break;
    case BACKWARD_LEFT: rotateMotor(RIGHT_MOTOR, BACKWARD); rotateMotor(LEFT_MOTOR, STOP); break;
    case BACKWARD_RIGHT: rotateMotor(RIGHT_MOTOR, STOP); rotateMotor(LEFT_MOTOR, BACKWARD); break;
    case STOP: default: rotateMotor(RIGHT_MOTOR, STOP); rotateMotor(LEFT_MOTOR, STOP); break;
  }
}

// Function to serve the main HTML page
void handleRoot(AsyncWebServerRequest *request) {
  request->send_P(200, "text/html", htmlHomePage);
}

// WebSocket event handler
void onCarInputWebSocketEvent(AsyncWebSocket *server, AsyncWebSocketClient *client,
                              AwsEventType type, void *arg, uint8_t *data, size_t len) {
  if (type == WS_EVT_CONNECT) {
    Serial.printf("Client #%u connected\n", client->id());
  } else if (type == WS_EVT_DISCONNECT) {
    Serial.printf("Client #%u disconnected\n", client->id());
    moveCar(STOP); // Stop car if client disconnects
  } else if (type == WS_EVT_DATA) {
    AwsFrameInfo *info = (AwsFrameInfo*)arg;
    if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
      std::string msg((char *)data, len); // Parse message
      std::istringstream ss(msg);
      std::string key, value;
      getline(ss, key, ',');
      getline(ss, value, ',');
      int val = atoi(value.c_str()); // Convert value to int
      if (key == "MoveCar") {
        moveCar(val);
      } else if (key == "Speed") {
        // Set speed for both motors using separate PWM channels
        ledcWrite(PWMSpeedChannelA, val); // Left motor
        ledcWrite(PWMSpeedChannelB, val); // Right motor
      }
    }
  }
}

// Setup function
void setup() {
  Serial.begin(115200); // Start serial communication

  // Set motor pins as outputs
  for (auto &motor : motorPins) {
    pinMode(motor.pinEn, OUTPUT);
    pinMode(motor.pinIN1, OUTPUT);
    pinMode(motor.pinIN2, OUTPUT);
    digitalWrite(motor.pinIN1, LOW);
    digitalWrite(motor.pinIN2, LOW);
  }

  // Setup PWM for motor speed control - Updated for ESP32 2.0.11
  // Left motor (enA = pin 5)
  ledcSetup(PWMSpeedChannelA, PWMFreq, PWMResolution);
  ledcAttachPin(motorPins[LEFT_MOTOR].pinEn, PWMSpeedChannelA);
  
  // Right motor (enB = pin 23)
  ledcSetup(PWMSpeedChannelB, PWMFreq, PWMResolution);
  ledcAttachPin(motorPins[RIGHT_MOTOR].pinEn, PWMSpeedChannelB);

  // Start WiFi in AP mode
  WiFi.softAP(ssid, password);
  Serial.println("WiFi started");
  Serial.println(WiFi.softAPIP()); // Print AP IP address

  // Setup WebSocket and HTTP handlers
  wsCarInput.onEvent(onCarInputWebSocketEvent);
  server.addHandler(&wsCarInput);
  server.on("/", handleRoot); // Serve homepage
  server.onNotFound([](AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found"); // 404 handler
  });

  server.begin(); // Start server
}

// Loop function (empty as everything is event-driven)
void loop() {
  //
}