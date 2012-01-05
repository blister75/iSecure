#include <OneWire.h>

OneWire ds(2);

byte key[8];
int keyready=false;
const int Debug=true;
int checkPin[6]={3,4,5,6,7,8}
int coilPin[6]={14,15,16,17,18,19}
byte unlockkey[6][9]={0};


void setup(void)
{
  for(int x=0;x<6;x++){
    pinMode(checkOne[x],OUTPUT);
  }
  for(int x=9;x<16;x++){
    pinMode(x,OUTPUT);
  }
  Serial.begin(9600);
  if(Debug){
    Serial.println("Ready");
  }
}
void loop(void)
{
  getKey();
  if(keyready){
    checkKeys();
  }
  if(Debug){
    if(digitalRead(17)){
      reportSerial();
    }
  }
}

void getKey(void){
  byte present = 0;
  byte addr[8];
  int x;
  if ( !ds.search(addr)) {
      ds.reset_search();
      return;
  }

  if ( OneWire::crc8( addr, 7) != addr[7]) {
    if(Debug){  
      Serial.println("DEBUG1: CRC invalid");
    }
    return; 
  } 
  if(Debug){
    Serial.println("DEBUG1: KEY Found");
  }
  for(x=0;x<8;x++){
    key[x]=addr[x];
  }
  for(x=0;x<6;x++){
    KeyCheck=digitalRead(checkOne[x]);
    if(!KeyCheck){
      keyready=x;
      break;
    }
  }
  ds.reset_search();
  ds.reset();
}

void checkKeys(void){
  keyready=false;
  for(int x=0;x<6;x++){
  if(unlockkey[x][8]){
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
    setlock(x);
    unlockkey[x][8]=1;
    if(Debug){
      Serial.print(x);
      Serial.print(" Locked with key ");
      for(int z=0;z<8;z++){
        Serial.print(key[z], HEX);
      }
      Serial.println("");
    }
  }
  }
}

void unlock(int loc){
  digitalWrite(coilPin[loc], HIGH);
  delay(2000);
  digitalWrite(coilPin[loc], LOW);
}
void setlock(int loc){
  for(int z = 0;z<8;z++){
    unlockkey[loc][z]=key[z];
  }
}
void reportSerial(){
  if(Debug){
    for(int x=0;x<6;x++){
      Serial.print(x+" ");
      for(int z=0;z<8;z++){
        Serial.print(unlockkey[x][z],HEX);
      }
      if(unlockkey[x][8]){
        Serial.println(" - Locked");
      }else{
        Serial.println(" - Unlocked");
      }
    }
  }
}
