#include <OneWire.h>
#include <EEPROM.h>

#define totalBox 6

OneWire ds(2);


byte key[8];
const int Debug=true;
int coilPin[totalBox]={2,3,4,5,6,7};
byte unlockkey[totalBox][9]={0};
int greenLed=9;
int redLed=10;

void setup(void)
{
  for(int x=0;x<totalBox;x++){
    pinMode(coilPin[x],OUTPUT);
  }
  Serial.begin(9600);
  if(Debug){
    Serial.println("Ready");
  }
}
void loop(void)
{
  if(getkey()){
    checkKeys();
  }
  if(Debug){
    if(digitalRead(17)){
      reportSerial();
    }
  }
}

int getkey(void){
  byte present = 0;
  byte addr[8];
  int x;
  if ( !ds.search(addr)) {
      ds.reset_search();
      return false;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
    if(Debug){  
      Serial.println("DEBUG1: CRC invalid");
    }
    return false; 
  } 
  if(Debug){
    Serial.println("DEBUG1: KEY Found");
  }
  for(x=0;x<8;x++){
    key[x]=addr[x];
  }
  ds.reset_search();
  ds.reset();
  return true;
}

void checkKeys(void){
  for(int x=0;x<totalBox;x++){
    if(key[7]==unlockkey[x][7]){
      int verify=0;
      for(int y=0;y<7;y++){
        if(key[y]==unlockkey[x][y]){
          verify++;
        }
      }
      if(verify>6){
        unlock(x);
        return;
      }
    }
  }
  for(int x=0;x<totalBox;x++){
    if(!unlockkey[x][8]){
      setlock(x);
      return;
    }
  }
  Serial.println("No Free Boxes");
  digitalWrite(greenLed,LOW);
  for(int x=0;x<10;x++){
    digitalWrite(redLed,HIGH);
    delay(500);
    digitalWrite(redLed,LOW);
  }
}

void unlock(int loc){
  digitalWrite(coilPin[loc], HIGH);
  delay(1000);
  digitalWrite(coilPin[loc], LOW);
  unreg(loc);
}
void setlock(int loc){
  for(int z = 0;z<8;z++){
    unlockkey[loc][z]=key[z];
  }
  addEPPROM(loc);
}
void reportSerial(){
  for(int x=0;x<totalBox;x++){
    if(Debug){Serial.print(x+" ");}
    for(int z=0;z<8;z++){
      if(Debug){Serial.print(unlockkey[x][z],HEX);}
    }
    if(unlockkey[x][8]){
      digitalWrite(coilPin[x],HIGH);
      if(Debug){Serial.println(" - Locked");}
    }else{
      if(Debug){Serial.println(" - Unlocked");}
    }
  }
  delay(1000);
  for(int x=0;x<totalBox;x++){
    digitalWrite(coilPin[x],LOW);
  }
}

void unreg(int loc){
  if(getkey()){
    digitalWrite(greenLed, HIGH);
    delay(750);
    digitalWrite(greenLed, LOW);
    int che=ds.reset();
    digitalWrite(redLed, HIGH);
    delay(750);
    digitalWrite(redLed, LOW);
    if(getkey() && !che){
      if(Debug){
        Serial.println(loc+" Unregistered");
      }
      unlockkey[loc][8]=0;
      removeEPPROM(loc);
    }
  }
}

void addEPPROM(int box){
  int addr;
  for(int x = 0;x<9;x++){
    addr=(box*9)+x;
    EEPROM.write(addr, unlockkey[box][x]);
  }
}

void removeEPPROM(int box){
    int addr=(box*9)+8;
    EEPROM.write(addr, 0);
}

void loadall(){
  int val=0;
  int addr=0;
  int reload=EEPROM.read(8);
  if(reload<=0){
    while(!setdefaults()){
      digitalWrite(redLed,HIGH);
      digitalWrite(greenLed,LOW);
      delay(1000);
      digitalWrite(redLed,LOW);
      digitalWrite(greenLed,HIGH);
    }
  }else{
    for(int y=0;y<totalBox;y++){
      for(int z=0;z<9;z++){
        addr=(y*9)+z;
        val=EEPROM.read(addr);
        unlockkey[y][z]=val;
      }
    }
  }
}

int setdefaults(){
  while(!getkey()){
    delay(500);
  }
  for(int x=0;x<8;x++){
    unlockkey[0][x]=key[x];
  }
  while(!getkey()){
    delay(500);
  }
  if(!(key[7]==unlockkey[0][7])){
    return 0;
  }
  unlockkey[0][8]=1;
  return 1;
}
