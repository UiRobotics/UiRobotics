// Idevice:skit:motor-45:servo-90;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  String data = Serial.readString();
  String command;
  int value;
  
  //if (Serial.available()) {
    if (data[0] == 'I') {
      data = rmSemi(data); // removes Idevice: and similar I: things
    }
    data = rmSemi(data); // removes the destination
    while (data.length() > 0) {
      data = parseM(data, command, value);
      // put command and value switches here
      Serial.println("command,value = " + command + " " + value);
      
    }
  //}
}

String parseM(String data, String &command, int &value) {
  command = "";
  value = 0;
  int i = 0;
  while (data[i] != '-') {
    command += data[i];
    i++;
  }
  String temp = "";
  i++;
  while (data[i] != ':' && data[i] != ';') {
    temp += data[i];
    value = temp.toInt();
    i++;
  }
  if (data[i] == ';') {
    for (i; i < (data.length())-1; i++) {
    }
  }
  data.remove(0,i+1);
  return data;
}

String rmSemi(String inputString) {
  while ((inputString[0] != ':') && inputString.length() > 0) {
    inputString.remove(0,1);
  }
  inputString.remove(0,1);
  return inputString;
}

