#include <ServoTimer2.h>
#include <IRremote.h>
unsigned char Forward[]={0x00, 0x00, 0x20, 0x50, 0x28, 0x14, 0x0a, 0x05, 0x05, 0x0a, 0x14, 0x28, 0x50, 0x20, 0x00, 0x00};
unsigned char Back[]={0x00, 0x01, 0x02, 0x05, 0x0a, 0x14, 0x28, 0x50, 0x50, 0x28, 0x14, 0x0a, 0x05, 0x02, 0x01, 0x00};
unsigned char Left[]={0x00, 0x00, 0x00, 0x18, 0x24, 0x5a, 0xa5, 0x42, 0x81, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
unsigned char Right[]={0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x81, 0x42, 0xa5, 0x5a, 0x24, 0x18, 0x00, 0x00, 0x00};
unsigned char Stop[]={0x00, 0x00, 0x00, 0x00, 0x7f, 0x41, 0x5d, 0x5d, 0x5d, 0x41, 0x7f, 0x00, 0x00, 0x00, 0x00, 0x00};
char memory[30]={'N', 'F', 'B', 'R','L', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N', 'N'};
unsigned int time[30]={0, 3000, 3000, 7000, 7000, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
#define lctrl 4
#define lpwm 5
#define rctrl 2
#define rpwm 9
#define speed 128
#define trigp 12
#define echop 13
#define ledp 3
#define SCL A4
#define SDA A5
#define irrecp A0
#define uperiod 100
#define stopDist 22
ServoTimer2 s;
unsigned long timer1 = 0, timer2 = 0, timer3 = 0;
char dangerway ='N', value, pval, buffer;
int dist, index = 0, recind = 0; 
bool memFollow = false, record = false;
void setup(){
  pinMode(lctrl, OUTPUT);
  pinMode(lpwm, OUTPUT);
  pinMode(rctrl, OUTPUT);
  pinMode(rpwm, OUTPUT);
  pinMode(trigp, OUTPUT);
  pinMode(echop, INPUT);
  pinMode(SCL, OUTPUT);
  pinMode(SDA, OUTPUT);
  pinMode(ledp, OUTPUT);
  s.attach(11);
  IrReceiver.begin(irrecp, false);
  Serial.begin(9600);
  stop();
}
void loop(){
  //ultrasonic
  if (millis()-timer1 >= uperiod && !record) {
    timer1 = millis();
    dist = measure();
    if (dist<stopDist && dangerway=='N'){
      dangerway = value;
      value = 'S';
    }
    else if (dist<stopDist && dangerway==value){
      value = 'S';
    }
    else if(dist>stopDist){
      dangerway ='N';
    }
  }
  //Bluetooth
  if (Serial.available()>0){
    value = Serial.read();
  }
  //IR reciever
  if (IrReceiver.decode()) {
      switch (IrReceiver.decodedIRData.decodedRawData) {
        case 0xB946FF00: value = 'F';break;
        case 0xBC43FF00: value = 'R';break;
        case 0xBB44FF00: value = 'L';break;
        case 0xEA15FF00: value = 'B';break;
        case 0xBF40FF00: value = 'S';break;
        case 0xBD42FF00: value = 'a';break;
        case 0xB54AFF00: value = 'd';break;
        case 0xF20DFF00: value = 'X';break;
        case 0xE916FF00: value = 'Q';break;
        case 0xE619FF00: value = 's';break;
      }
      IrReceiver.resume();
  }
  //follow comands
  if (memFollow && pval==value) {
    if (millis()-timer2 >= time[index]){
      timer2 = millis();
      index++;
      if (memory[index] == 'N'){
        value = 'S';
        memFollow = false;
        index = 0;
        digitalWrite(3, LOW);
      }
      else{
        value = memory[index];
      }
    }
  }
  else if (pval != value){
    memFollow = false;
    index = 0;
    digitalWrite(3, LOW);
  }
  //recording path
  if(record){
    if (pval != value) {
      if(value == 's'){
        time[recind] = millis() - timer3;
        record = false;
        for(int i=recind; i<15; i++){
          memory[i] = 'N';
        }
        recind = 0;
        digitalWrite(3, LOW);
      }
      else{
        time[recind] = millis() - timer3;
        recind++;
        memory[recind] = value;
        timer3 = millis();
      }
    }
  }
  //direction choise
  if (pval != value) {
     switch(value){
      case 'F': forward();break;
      case 'B': back();break;
      case 'L': left();break;
      case 'R': right();break;
      case 'S': stop();break;
      case 'a': leftback();break;
      case 'd': rightback(); break;
      case 'X': memFollow=true;record=false;timer2=millis();digitalWrite(3, HIGH);break;
      case 'Q': record=true;memFollow=false;timer3=millis();digitalWrite(3, HIGH);break;
    }
  }
  pval=value;
}
void forward(){
  digitalWrite(lctrl, HIGH);
  analogWrite(lpwm, speed);
  digitalWrite(rctrl, HIGH);
  analogWrite(rpwm, speed);
  matrix(Forward);
  s.write(1500);
}
void back(){
  digitalWrite(lctrl, LOW);
  analogWrite(lpwm, speed);
  digitalWrite(rctrl, LOW);
  analogWrite(rpwm, speed);
  matrix(Back);
  s.write(1500);
}
void left(){
  digitalWrite(rctrl, HIGH);
  analogWrite(rpwm, 200);
  analogWrite(lpwm, 0);
  matrix(Left);
  s.write(2260);
}
void leftback(){
  digitalWrite(rctrl, LOW);
  analogWrite(rpwm, 200);
  analogWrite(lpwm, 0);
  matrix(Left);
  s.write(1500);
}
void right(){
  digitalWrite(lctrl, HIGH);
  analogWrite(lpwm, 200);
  analogWrite(rpwm, 0);
  matrix(Right);
  s.write(720);
}
void rightback(){
  digitalWrite(lctrl, LOW);
  analogWrite(lpwm, 200);
  analogWrite(rpwm, 0);
  matrix(Right);
  s.write(1500);
}
void stop(){
  analogWrite(lpwm, 0);
  analogWrite(rpwm, 0);
  matrix(Stop);
}
int measure(){
  digitalWrite(trigp, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigp, LOW);
  long duration = pulseIn(echop, HIGH);
  int cm = duration * 0.0343 / 2;
  return cm;
}
void matrix(unsigned char data[]){
  iccstart();
  send(0xc0);
  for(int i=0; i<16; i++){
    send(data[i]);
  }
  iccend();
  iccstart();
  send(0x8A);
  iccend();
}
void iccstart(){
  digitalWrite(SCL, HIGH);
  delayMicroseconds(3);
  digitalWrite(SDA, HIGH);
  delayMicroseconds(3);
  digitalWrite(SDA, LOW);
  delayMicroseconds(3);
}
void send(unsigned char data){
  for(int i=0; i<8; i++){
    digitalWrite(SCL, LOW);
    delayMicroseconds(3);
    if(data & 0x01){
      digitalWrite(SDA, HIGH);
    }
    else{
      digitalWrite(SDA, LOW);
    }
    digitalWrite(SCL, HIGH);
    delayMicroseconds(3);
    data = data >> 1;
  }
}
void iccend(){
  digitalWrite(SCL, LOW);
  delayMicroseconds(3);
  digitalWrite(SDA, LOW);
  delayMicroseconds(3);
  digitalWrite(SCL, HIGH);
  delayMicroseconds(3);
  digitalWrite(SDA, HIGH);
  delayMicroseconds(3);
}