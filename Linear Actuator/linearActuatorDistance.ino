#include <Servo.h>
Servo myservo;

void setup () {
  myservo.attach(9); 
  Serial.begin(9600);
}

void loop() {  
  // 0 means full power in one direction.
  // Actually the minimum value for me is around 30.
  // A smaller value won't drive the motor.
  float data, input;
  input = 1.3;
  //myservo.write(85);
  data = readpot(); 
 while(data < input-.01 || data > input+.01){
  data = readpot();
    if(data < input){
      myservo.write(110);
    }
    else{
      myservo.write(85);
    }
 }
 myservo.write(90);
  //delay (300);
 
  
  // 90 means stopping the motor.
 // myservo.write(90); 
  //delay(2000);  
    
  // 180 means full power in the other direction.
  // Actually the maximum value for me is around 160.
  // A larger value won't drive the motor either.
  //myservo.write(160); 
  //delay(2000);
}





// .77 to 4.87
//yellow to A1
//white 5v
//blue ground

float readpot (){
  int whiteVal = analogRead(A1);

  //float vw = whiteVal * (5.0 / 1023.0);

  float x = map(analogRead(A1),26,876,0,3690);
  float y = x/1000;
  float z = constrain(y,0,3.69);

  Serial.println(z);
  return z;
 
  delay(300);
}


