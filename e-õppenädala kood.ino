#include <Servo.h>
Servo myservo;

const int buttonPin = 8;
int buttonState = 0;
int lastButtonState = 0;

// annab servole kiiruse
int speed = 1500; 
bool rotatingClockwise = true;

void setup() {
  Serial.begin(9600); 

  // servo on ühendatud 9nda pin külge
  myservo.attach(9); 
  pinMode(buttonPin, INPUT);
  myservo.writeMicroseconds(1500); 
}

void loop() {
  buttonState = digitalRead(buttonPin);

  // kontrollob kas nupp on vajutatud või ei ole ja saadab info serial monitori
  if (buttonState == HIGH && lastButtonState == LOW) {
    delay(50); 
    rotatingClockwise = !rotatingClockwise; 
    if (rotatingClockwise) {
      Serial.println("Servo liigub paripaeva");
      speed = 1600; 
    } else {
      Serial.println("Servo liigub vastupaeva");
      speed = 1400; 
    }
  }
  lastButtonState = buttonState;
  myservo.writeMicroseconds(speed); 
  delay(20);
}