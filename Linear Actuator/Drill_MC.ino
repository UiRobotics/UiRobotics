#include <Servo.h>
// Sabertooth 2x32
// Dipswitches 2 and 5 are up
Servo LAD;
Servo motor;
void setup() {
   LAD.attach(9);
   motor.attach(8);
   Serial.begin(9600);
   pinMode(2,OUTPUT);
   digitalWrite(2,HIGH);
}

// motor 90-98 is stopped
// lad potentiometer is 100-990
// lad wires: 
void loop() {
  char key;
  char dist[4];
  if (Serial.available()) {
    String input = Serial.readString();
    key = input[0];
    if (input[3] == '.') {
      for (int i = 2; i < input.length(); i++) {
         dist[i-2] = input[i];
         dist[i-1] = '\0';
      }
    }
    if (key == 'f') {
     motor.write(100); 
    }
    else if (key == 's') {
     motor.write(87); 
    }
    else {
     motor.write(92);
    }
    moveto(atof(dist));
  }
}

void moveto(float input) {
  float data;
  data = readpot();
  while (data < input - .02 || data > input + .02) {
    data = readpot();
    if (data < input - .02) {
      LAD.write(40);
    }
    else if (data > input + .02) {
      LAD.write(120);
    }
    else {
      LAD.write(90);
    }
  }
}

float readpot() {
  float x = map(analogRead(A1), 100, 990, 0, 3690);
  float y = x / 1000;
  float z = constrain(y, 0, 3.69);
  return z;
  delay(300);
}
