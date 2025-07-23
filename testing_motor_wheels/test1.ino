// Pines del L298N - CON MOTORES INTERCAMBIADOS
const int IN1 = 22;  // Ahora controla el motor FÍSICO IZQUIERDO
const int IN2 = 21;  
const int IN3 = 19;  // Ahora controla el motor FÍSICO DERECHO
const int IN4 = 18;
const int enA = 5;   // PWM para motor FÍSICO IZQUIERDO
const int enB = 23;  // PWM para motor FÍSICO DERECHO

// Configuración PWM
const int freq = 5000;     // Frecuencia en Hz
const int resolution = 8;  // 8 bits (0-255)
const int testSpeed = 180; // Velocidad de prueba (100-255)

void setup() {
  Serial.begin(115200);
  Serial.println("Prueba de motores - Versión final intercambiada");
  
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
  testPhysicalRightWheel(); // Controla el motor FÍSICO derecho (pines IN3/IN4)
  testPhysicalLeftWheel();  // Controla el motor FÍSICO izquierdo (pines IN1/IN2)
  delay(2000);
}

// Control del motor FÍSICO derecho (con polaridad corregida)
void testPhysicalRightWheel() {
  Serial.println("Motor FÍSICO derecho (IN3/IN4) adelante");
  digitalWrite(IN3, LOW);   // Polaridad que ya funcionaba
  digitalWrite(IN4, HIGH);
  ledcWrite(enB, testSpeed); // Usa enB para el motor derecho
  delay(3000);
  
  Serial.println("Motor FÍSICO derecho atrás");
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  ledcWrite(enB, testSpeed);
  delay(3000);
  
  stopAllMotors();
  delay(1000);
}

// Control del motor FÍSICO izquierdo (con polaridad corregida)
void testPhysicalLeftWheel() {
  Serial.println("Motor FÍSICO izquierdo (IN1/IN2) adelante");
  digitalWrite(IN1, HIGH);  // Polaridad que ya funcionaba
  digitalWrite(IN2, LOW);
  ledcWrite(enA, testSpeed); // Usa enA para el motor izquierdo
  delay(3000);
  
  Serial.println("Motor FÍSICO izquierdo atrás");
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  ledcWrite(enA, testSpeed);
  delay(3000);
  
  stopAllMotors();
  delay(1000);
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
