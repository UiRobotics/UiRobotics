/* 
Wiring:
  Pin_GND #2 On SF02 Laser Rangefinder - GND (Vss On Some Boards)
  Pin_+5V #3 On SF02 Laser Rangefinder - +5V (Vdd On Some Boards)
  Pin_TXD #8 On SF02 Laser Rangefinder - Arduino RX Pin (10)
  Pin_RXD #9 On SF02 Laser Rangefinder - Arduino TX Pin (11)
*/
#include <SoftwareSerial.h>                                            

#define terminal_baud_rate    9600
#define serial_Rxd            10
#define serial_Txd            11
#define sf02_baud_rate        9600 // set in the sf02 terminal application

SoftwareSerial sf02_serial(serial_Rxd, serial_Txd); // sets the sf02 serial port connection

float distance;
char sf02_string[16], c;
float slope;

void setup() {
  Serial.begin(terminal_baud_rate);
  sf02_serial.begin(sf02_baud_rate); // opens the sf02 serial port
}

void loop(){
  sf02_serial.write("d"); // send 'd' to the sf02 which is the trigger for it to send back the distance
  while (!sf02_serial.available()); // waits for the next distance measurement
  int i = 0, c = 0;
  
  while(c != 10) { // reads the ASCII string from the sf02 until (\n) is detected
    while (!sf02_serial.available()); // waits for the next character
    c = sf02_serial.read();
    sf02_string[i] = c;
    i++;
  }
  sf02_string[i-2] = 0;
  distance = atof(sf02_string); // converts the ASCII string to meters
  Serial.println(sf02_string);
  delay(100);
}
