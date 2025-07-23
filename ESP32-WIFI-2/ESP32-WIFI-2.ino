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
      padding: 0;
      overflow: hidden;
      display: flex;
      justify-content: center;
      align-items: center;
      height: 100vh;
    }
    .layout {
      display: flex;
      flex-direction: row;
      justify-content: space-between;
      width: 90%;
      max-width: 600px;
      align-items: center;
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
      width: 120px;
      height: 120px;
      background-color: white;
      border-radius: 10px;
      box-shadow: 5px 5px #888888;
      display: flex;
      justify-content: center;
      align-items: center;
      user-select: none;
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
  </style>
</head>
<body>
  <!-- Main layout with buttons and slider -->
  <div class="layout">
    <div class="column">
      <div class="button" ontouchstart='send("MoveCar","1")' ontouchend='send("MoveCar","0")' onmousedown='send("MoveCar","1")' onmouseup='send("MoveCar","0")'>UP</div>
      <div class="button" ontouchstart='send("MoveCar","2")' ontouchend='send("MoveCar","0")' onmousedown='send("MoveCar","2")' onmouseup='send("MoveCar","0")'>DOWN</div>
    </div>
    <div class="right-controls">
      <div class="slider-container">
        <label for="Speed">Speed:</label><br>
        <input type="range" min="0" max="255" value="150" class="slider" id="Speed" oninput='send("Speed",value)'>
      </div>
      <div class="row">
        <div class="button" ontouchstart='send("MoveCar","3")' ontouchend='send("MoveCar","0")' onmousedown='send("MoveCar","3")' onmouseup='send("MoveCar","0")'>LEFT</div>
        <div class="button" ontouchstart='send("MoveCar","4")' ontouchend='send("MoveCar","0")' onmousedown='send("MoveCar","4")' onmouseup='send("MoveCar","0")'>RIGHT</div>
      </div>
    </div>
  </div>

  <script>
    var socket;
    // Establish WebSocket connection
    function connect() {
      socket = new WebSocket("ws://" + location.host + "/CarInput");
      socket.onopen = () => {
        send("Speed", document.getElementById("Speed").value); // Set initial speed
      };
      socket.onclose = () => setTimeout(connect, 2000); // Reconnect if disconnected
    }

    // Send key-value pair through WebSocket
    function send(key, value) {
      if (socket && socket.readyState === WebSocket.OPEN) {
        socket.send(key + "," + value);
      }
    }

    window.onload = connect; // Connect on page load
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