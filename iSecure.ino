#include <OneWire.h>

OneWire ds1(5);
OneWire ds2(6);  
OneWire ds3(7);
int groundOnewire[4]={1,2,3,4};
int xSol=7;
int ySol=10;
byte key[8];
int location[2]={0};
int keyready=false;
const int Debug=true;
byte unlockkey[3][4][9]={0};

void setup(void)
{
  for(int x=0;x<4;x++){
    pinMode(groundOnewire[x],OUTPUT);
  }
  Serial.begin(9600);
}
void loop(void)
{
  for(int x=0;x<4;x++){
    location[0]=1;
    if(!keyready){
      digitalWrite(groundOnewire[x],LOW);
      getKey1();
      location[1]=x;
      digitalWrite(groundOnewire[x],HIGH);
    }
  }
  for(int x=0;x<4;x++){
    location[0]=2;
    if(!keyready){
      digitalWrite(groundOnewire[x],LOW);
      getKey2();
      location[1]=x;
      digitalWrite(groundOnewire[x],HIGH);
    }
  }
  for(int x=0;x<4;x++){
    location[0]=3;
    if(!keyready){
      digitalWrite(groundOnewire[x],LOW);
      getKey3();
      location[1]=x;
      digitalWrite(groundOnewire[x],HIGH);
    }
  }
  if(keyready){
    checkKeys();
  }
  if(Debug){
    if(digitalRead(15)){
      reportSerial();
    }
  }
}

void getKey1(void){
  byte present = 0;
  byte addr[8];
  int x;
  if ( !ds1.search(addr)) {
      ds1.reset_search();
      return;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
    if(Debug){  
      Serial.print("DEBUG1: CRC invalid");
    }
    return; 
  } 
  if(Debug){
    Serial.print("DEBUG1: KEY Found");
  }
  for(x=0;x<8;x++){
    key[x]=addr[x];
  }
  keyready=true;
  ds1.reset_search();
  ds1.reset();
}

void getKey2(void){
  byte present = 0;
  byte addr[8];
  int x;
  if ( !ds2.search(addr)) {
      ds2.reset_search();
      return;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
    if(Debug){  
      Serial.print("DEBUG2: CRC invalid");
    }
    return; 
  } 
  if(Debug){
    Serial.print("DEBUG2: KEY Found");
  }
  for(x=0;x<8;x++){
    key[x]=addr[x];
  }
  keyready=true;
  ds2.reset_search();
  ds2.reset();
}
void getKey3(void){
  byte present = 0;
  byte addr[8];
  int x;
  if ( !ds3.search(addr)) {
      ds3.reset_search();
      return;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
    if(Debug){  
      Serial.print("DEBUG3: CRC invalid");
    }
    return; 
  } 
  if(Debug){
    Serial.print("DEBUG3: KEY Found");
  }
  for(x=0;x<8;x++){
    key[x]=addr[x];
  }
  keyready=true;
  ds3.reset_search();
  ds3.reset();
}

void checkKeys(void){
  int x=location[0];
  int y=location[1];
  if(unlockkey[x][y][8]){
    int verify=0;
    for(int z=0;z<8;z++){
      if(key[z]==unlockkey[x][y][z]){
        verify++;
      }
    }
    if(verify=8){
      unlock(x,y);
      unlockkey[x][y][8]=0;
      if(Debug){
        Serial.print(x);
        Serial.print(", ");
        Serial.print(y);
        Serial.println(" Unlocked");
      }
    }
  }else{
    setlock(x,y);
    unlockkey[x][y][8]=1;
    if(Debug){
      Serial.print(x);
      Serial.print(", ");
      Serial.print(y);
      Serial.print(" Locked with key ");
      for(int z=0;z<8;z++){
        Serial.print(key[z]);
      }
      Serial.println("");
    }
  }
}

void unlock(int boxX, int boxY){
  boxX += xSol;
  boxY += ySol;
  digitalWrite(boxX, HIGH);
  digitalWrite(boxY, LOW);
  delay(2000);
  digitalWrite(boxX, LOW);
  digitalWrite(boxY, HIGH);
}
void setlock(int boxX, int boxY){
  for(int z = 0;z<8;z++){
    unlockkey[boxX][boxY][z]=key[z];
  }
}
void reportSerial(){
  if(Debug){
    for(int x=0;x<3;x++){
      for(int y=0;y<4;y++){
        Serial.print(x+", "+y);
        for(int z=0;z<8;z++){
          Serial.print(unlockkey[x][y][z],HEX);
        }
        if(unlockkey[x][y][8]){
          Serial.println(" - Locked");
        }else{
          Serial.println(" - Unlocked");
        }
      }
    }
  }
}
