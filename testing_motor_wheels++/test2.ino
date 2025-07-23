// Pines del L298N - Configuración intercambiada
const int IN1 = 22;  // Control motor físico izquierdo
const int IN2 = 21;  
const int IN3 = 19;  // Control motor físico derecho
const int IN4 = 18;
const int enA = 5;   // PWM motor izquierdo
const int enB = 23;  // PWM motor derecho

// Configuración PWM
const int freq = 5000;
const int resolution = 8;
const int minSpeed = 80;   // Velocidad mínima para movimiento
const int maxSpeed = 220;  // Velocidad máxima recomendada
const int incremento = 10; // Paso de incremento de velocidad
const int delayIncremento = 200; // ms entre incrementos

void setup() {
  Serial.begin(115200);
  Serial.println("Prueba con control gradual de velocidad");
  
  // Configurar pines
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  
  // Configurar PWM
  ledcAttach(enA, freq, resolution);
  ledcAttach(enB, freq, resolution);
  
  stopAllMotors();
}

void loop() {
  testLeftWheelWithRamp();
  testRightWheelWithRamp();
  delay(2000);
}

// Prueba con aceleración gradual para motor izquierdo
void testLeftWheelWithRamp() {
  // Adelante con aceleración
  Serial.println("Motor izquierdo: Acelerando adelante");
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  accelerateMotor(enA, minSpeed, maxSpeed);
  
  // Decelerar
  decelerateMotor(enA, maxSpeed, minSpeed);
  
  // Atrás con aceleración
  Serial.println("Motor izquierdo: Acelerando atrás");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  accelerateMotor(enA, minSpeed, maxSpeed);
  
  // Decelerar
  decelerateMotor(enA, maxSpeed, minSpeed);
  
  stopAllMotors();
  delay(1000);
}

// Prueba con aceleración gradual para motor derecho
void testRightWheelWithRamp() {
  // Adelante con aceleración
  Serial.println("Motor derecho: Acelerando adelante");
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  accelerateMotor(enB, minSpeed, maxSpeed);
  
  // Decelerar
  decelerateMotor(enB, maxSpeed, minSpeed);
  
  // Atrás con aceleración
  Serial.println("Motor derecho: Acelerando atrás");
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  accelerateMotor(enB, minSpeed, maxSpeed);
  
  // Decelerar
  decelerateMotor(enB, maxSpeed, minSpeed);
  
  stopAllMotors();
  delay(1000);
}

// Función para aceleración gradual
void accelerateMotor(int motorPin, int startSpeed, int endSpeed) {
  for(int speed = startSpeed; speed <= endSpeed; speed += incremento) {
    ledcWrite(motorPin, speed);
    Serial.print("Velocidad: ");
    Serial.println(speed);
    delay(delayIncremento);
  }
}

// Función para desaceleración gradual
void decelerateMotor(int motorPin, int startSpeed, int endSpeed) {
  for(int speed = startSpeed; speed >= endSpeed; speed -= incremento) {
    ledcWrite(motorPin, speed);
    Serial.print("Velocidad: ");
    Serial.println(speed);
    delay(delayIncremento);
  }
}

void stopAllMotors() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
  ledcWrite(enA, 0);
  ledcWrite(enB, 0);
  Serial.println("Motores detenidos");
}