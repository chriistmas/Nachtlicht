#include "BluetoothSerial.h"   // Include Bluetooth library
#include <Arduino.h>           // Include Arduino core library

BluetoothSerial serialBT;      // Create Bluetooth serial object

char incoming_signal;          // Variable to store incoming Bluetooth command

int Speed = 100;               // Initial motor speed

// Motor control pins - Using your working configuration
const int IN1 = 22;  // Control motor físico izquierdo
const int IN2 = 21;  
const int IN3 = 19;  // Control motor físico derecho
const int IN4 = 18;
const int enA = 5;   // PWM motor izquierdo
const int enB = 23;  // PWM motor derecho

// PWM Configuration - Using your working settings
const int freq = 5000;
const int resolution = 8;
const int minSpeed = 80;   // Velocidad mínima para movimiento
const int maxSpeed = 220;  // Velocidad máxima recomendada

void setup() {
  Serial.begin(115200);             // Start serial communication for debugging
  serialBT.begin("Aslam Hossain YT");       // Start Bluetooth with name "Aslam Hossain YT"

  // Configure pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // Configure PWM using your working method
  ledcAttach(enA, freq, resolution);
  ledcAttach(enB, freq, resolution);

  // Ensure motors are stopped initially
  stop();
  
  Serial.println("Bluetooth Mini Soccer Robot Ready");
}

void loop() {
  // Check if a Bluetooth command is received
  while (serialBT.available()) {
    incoming_signal = serialBT.read();   // Read the command
    Serial.println(incoming_signal);     // Print for debugging

    // Speed control commands - Constrained to your working range
    if (incoming_signal == '0') Speed = minSpeed;
    else if (incoming_signal == '1') Speed = 90;
    else if (incoming_signal == '2') Speed = 100;
    else if (incoming_signal == '3') Speed = 110;
    else if (incoming_signal == '4') Speed = 120;
    else if (incoming_signal == '5') Speed = 130;
    else if (incoming_signal == '6') Speed = 150;
    else if (incoming_signal == '7') Speed = 170;
    else if (incoming_signal == '8') Speed = 190;
    else if (incoming_signal == '9') Speed = 210;
    else if (incoming_signal == 'q') Speed = maxSpeed;

    // Movement commands
    else if (incoming_signal == 'F') forward();         // Forward
    else if (incoming_signal == 'B') backward();        // Backward
    else if (incoming_signal == 'L') left();            // Left
    else if (incoming_signal == 'R') right();           // Right
    else if (incoming_signal == 'S') stop();            // Stop
    else if (incoming_signal == 'I') forward_right();   // Forward-right
    else if (incoming_signal == 'G') forward_left();    // Forward-left
    else if (incoming_signal == 'J') backward_right();  // Backward-right
    else if (incoming_signal == 'H') backward_left();   // Backward-left
  }
}

// Move both motors forward
void forward() {
  ledcWrite(enA, Speed);  // Left motor
  ledcWrite(enB, Speed);  // Right motor

  // Left motor forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  // Right motor forward
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// Move both motors backward
void backward() {
  ledcWrite(enA, Speed);  // Left motor
  ledcWrite(enB, Speed);  // Right motor

  // Left motor backward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  // Right motor backward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// Turn left (right motor forward, left motor backward)
void left() {
  ledcWrite(enA, Speed);  // Left motor
  ledcWrite(enB, Speed);  // Right motor

  // Left motor backward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  // Right motor forward
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// Turn right (left motor forward, right motor backward)
void right() {
  ledcWrite(enA, Speed);  // Left motor
  ledcWrite(enB, Speed);  // Right motor

  // Left motor forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  // Right motor backward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// Stop all motor movement
void stop() {
  ledcWrite(enA, 0);
  ledcWrite(enB, 0);

  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// Move forward and slightly right (left motor normal, right motor reduced)
void forward_right() {
  ledcWrite(enA, Speed);      // Left motor normal
  ledcWrite(enB, Speed/2);    // Right motor slower

  // Left motor forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  // Right motor forward
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// Move forward and slightly left (right motor normal, left motor reduced)
void forward_left() {
  ledcWrite(enA, Speed/2);    // Left motor slower
  ledcWrite(enB, Speed);      // Right motor normal

  // Left motor forward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  // Right motor forward
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

// Move backward and slightly right (left motor normal, right motor reduced)
void backward_right() {
  ledcWrite(enA, Speed);      // Left motor normal
  ledcWrite(enB, Speed/2);    // Right motor slower

  // Left motor backward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  // Right motor backward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

// Move backward and slightly left (right motor normal, left motor reduced)
void backward_left() {
  ledcWrite(enA, Speed/2);    // Left motor slower
  ledcWrite(enB, Speed);      // Right motor normal

  // Left motor backward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  // Right motor backward
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}