// ==== RC PWM muutujad ====
volatile uint32_t ch1Start = 0;
volatile uint32_t ch3Start = 0;
volatile uint16_t ch1Pulse = 1500; // rool
volatile uint16_t ch3Pulse = 1500; // sõit

// ==== PINNID ====
// Keeramine ees (1 mootor, ENA jumperiga)
const int STEER_IN1 = 6;
const int STEER_IN2 = 7;
const int CH1_PIN = 2;

// Sõidumootorid taga (2 mootorit, ENB PWM 9)
const int DRIVE_IN1 = 10;
const int DRIVE_IN2 = 11;
const int DRIVE_EN = 9;
const int CH3_PIN = 3;

// Täpne deadzone ±50 µs
const int DEAD_LOW = 1450;
const int DEAD_HIGH = 1550;

// Maksimaalne PWM
const int DRIVE_POWER = 255;

// ==== ISR ====
void ISR_CH1() {
  static bool last = LOW;
  bool now = digitalRead(CH1_PIN);
  uint32_t t = micros();
  if (now && !last) ch1Start = t;
  if (!now && last) ch1Pulse = t - ch1Start;
  last = now;
}

void ISR_CH3() {
  static bool last = LOW;
  bool now = digitalRead(CH3_PIN);
  uint32_t t = micros();
  if (now && !last) ch3Start = t;
  if (!now && last) ch3Pulse = t - ch3Start;
  last = now;
}

// ==== SETUP ====
void setup() {
  pinMode(CH1_PIN, INPUT);
  pinMode(CH3_PIN, INPUT);

  pinMode(STEER_IN1, OUTPUT);
  pinMode(STEER_IN2, OUTPUT);

  pinMode(DRIVE_IN1, OUTPUT);
  pinMode(DRIVE_IN2, OUTPUT);
  pinMode(DRIVE_EN, OUTPUT);

  attachInterrupt(digitalPinToInterrupt(CH1_PIN), ISR_CH1, CHANGE);
  attachInterrupt(digitalPinToInterrupt(CH3_PIN), ISR_CH3, CHANGE);

  Serial.begin(115200);
}

// ==== LOOP ====
void loop() {
  // ---- Loe RC kanalid ----
  noInterrupts();
  uint16_t steer = ch1Pulse;
  uint16_t drive = ch3Pulse;
  interrupts();

  // ---- Keeramine ees (CH1) ----
  if (steer < DEAD_LOW) {          // vasak
    digitalWrite(STEER_IN1, HIGH);
    digitalWrite(STEER_IN2, LOW);
  }
  else if (steer > DEAD_HIGH) {    // parem
    digitalWrite(STEER_IN1, LOW);
    digitalWrite(STEER_IN2, HIGH);
  }
  else {                           // keskel → stopp
    digitalWrite(STEER_IN1, LOW);
    digitalWrite(STEER_IN2, LOW);
  }

  // ---- Sõit taga (CH3) ----
  if (drive < DEAD_LOW) {          // tagurpidine
    digitalWrite(DRIVE_IN1, LOW);
    digitalWrite(DRIVE_IN2, HIGH);
    analogWrite(DRIVE_EN, DRIVE_POWER);
  }
  else if (drive > DEAD_HIGH) {    // edasi
    digitalWrite(DRIVE_IN1, HIGH);
    digitalWrite(DRIVE_IN2, LOW);
    analogWrite(DRIVE_EN, DRIVE_POWER);
  }
  else {                           // keskel → stopp
    digitalWrite(DRIVE_IN1, LOW);
    digitalWrite(DRIVE_IN2, LOW);
    analogWrite(DRIVE_EN, 0);
  }

  // ---- Serial Monitor debug ----
  Serial.print("Steer=");
  Serial.print(steer);
  Serial.print("  Drive=");
  Serial.println(drive);

  delay(20); // ~50Hz RC kaadri sagedus
}