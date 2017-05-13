#include <EEPROM.h>
#include <IRremote.h>
#include <Keypad.h>
#include <Keyboard.h>
#include <Mouse.h>
//#define USB_HID_PROTOCOL_KEYBOARD 0x01
#define ROWS 4
#define COLS 4
#define PC_CONTROL 84
IRrecv Receiver(7);
IRsend Sender;
void sendCode(char);
void pcControlStoreCodes();
void pcControlMode();
byte getAdr(char);
byte findCode(long unsigned);
byte toggle = 0;
const char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
  
};
const byte rowPins[ROWS] = {2, 10, 9, 6}; //connect to the row pinouts of the keypad
const byte colPins[COLS] = {8, 5, 4, 3}; //connect to column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

void setup()
{
  Receiver.enableIRIn();
  Serial.begin(9600);
  Serial1.begin(9600);
  Keyboard.begin();
  Mouse.begin();
}

void loop() {
  if(Serial1.available()){
    Serial.write(Serial1.read());
    Serial1.write("+++\n");
  }


  delay(10);
char * msg;
  if (keypad.getKeys())
    {
        for (byte i=0; i<LIST_MAX; i++)   // Scan the whole key list.
        {
            if ( keypad.key[i].stateChanged )   // Only find keys that have changed state.
            {
                switch (keypad.key[i].kstate) {  // Report active key state : IDLE, PRESSED, HOLD, or RELEASED
                    case PRESSED:{
                      msg = " PRESSED.";
                      if (keypad.key[i].kchar == 'A'){
                        for(int z=0;z<LIST_MAX;z++){
                          if((keypad.key[z].kchar == 'B') && (keypad.key[z].kstate == HOLD))
                          {
                              Serial.println("\nB+A");
                              storeCode(0);
                          }
                        }
                      }
                      if (keypad.key[i].kchar == 'C'){
                        for(int z=0;z<LIST_MAX;z++){
                          if((keypad.key[z].kchar == 'B') && (keypad.key[z].kstate == HOLD))
                          {
                              Serial.println("\nB+C");
                              storeCode(1);
                          }
                        }
                      }
                      if (keypad.key[i].kchar == '*'){
                        for(byte z=0;z<LIST_MAX;z++){
                          if((keypad.key[z].kchar == '#') && (keypad.key[z].kstate == HOLD))
                          {
                              Serial.println("\n*+#");
                              digitalWrite(13,HIGH);
                              delay(15000);
                          }
                        }
                      }
                      if (keypad.key[i].kchar == 'D'){
                        for(byte z=0;z<LIST_MAX;z++){
                          if((keypad.key[z].kchar == 'B') && (keypad.key[z].kstate == HOLD))
                          {
                              Serial.println("\nB+D");
                              pcControlStoreCodes();
                          }
                        }
                      }
                      if (keypad.key[i].kchar == '6'){
                        for(byte z=0;z<LIST_MAX;z++){
                          if((keypad.key[z].kchar == 'B') && (keypad.key[z].kstate == HOLD))
                          {
                              Serial.println("\nB+6");
                              pcControlMode();
                          }
                        }
                      }
                      sendCode(keypad.key[i].kchar);
                    }
                break;
                    case HOLD:
                    msg = " HOLD.";
                    /*while (keypad.getKeys() && !keypad.key[i].stateChanged) {
                      sendCode(keypad.key[i].kchar);
                      delay(400);
                    }*/
                break;
                    case RELEASED:{
                    msg = " RELEASED.";
                      
                    }
                break;
                    case IDLE:
                    msg = " IDLE.";
                }
                Serial.print("Key ");
                Serial.print(keypad.key[i].kchar);
                Serial.println(msg);
            }
        }
    }
}


void EEPROM_writeInt(int adr, unsigned long value)
{
   byte* p = (byte*)&value;
   for (int i = 0; i < 4; i++){
      EEPROM.write(adr, *p);
      Serial.print("EEPROM Write ");
      Serial.print(*p);
      Serial.print(" to ");
      Serial.println(adr);
      p++;
      adr++;
      
   }
       
}

long unsigned EEPROM_readInt(int adr)
{
   long unsigned value;
   byte* p = (byte*)&value;
   for (int i = 0; i < 4; i++){
      *p = EEPROM.read(adr);
      Serial.print("EEPROM Read ");
      Serial.print(*p);
      Serial.print(" from ");
      Serial.println(adr);
      p++;
      adr++;
      
   }
   return value;    
}

void readEEP(){
  for(int address=0;address < 150;address++){
  int value = EEPROM.read(address);
  Serial.print(address);
  Serial.print("\t");
  Serial.print(value, DEC);
  Serial.println();
  }
}

void EEPROM_writeRAW(int adr, unsigned int * arr, int len)
{
   byte* p = (byte*)arr;
   p+=2;
   for (int i = 2; i < len*2; i++){
      if (i%2) {
        p++;
        continue; //skip high byte
      }
      EEPROM.write(adr, *p);
      Serial.print("EEPROM Write ");
      Serial.print(*p);
      Serial.print(" to ");
      Serial.println(adr);
      p++;
      adr++;
      
   }
       
}
unsigned int * EEPROM_readRAW(int adr,int len)
{
   unsigned int * arr = (unsigned int *)malloc(len*sizeof(unsigned int));
   byte* p = (byte*)arr;
   for (int i = 0; i < len*2-2; i++){
      if (i%2) {
        *p = 0; //high byte
        p++;
        continue;
      }
      *p = EEPROM.read(adr);
      Serial.print("EEPROM Read ");
      Serial.print(*p);
      Serial.print(" from ");
      Serial.println(adr);
      p++;
      adr++;     
   }

   for (int i = 0; i < len-1; i++) {
      if (!(i % 2)) {
        // Mark
        arr[i] = arr[i]*USECPERTICK - MARK_EXCESS;
        Serial.print(" m");
      } 
      else {
        // Space
        arr[i] = arr[i]*USECPERTICK + MARK_EXCESS;
        Serial.print(" s");
      }
      Serial.print(arr[i], DEC);
    }
   return arr;    
}
void storeCode(byte storeRaw){
  Serial.println("1");
  char bindKey = keypad.waitForKey();
  Serial.println("2");
  byte address = getAdr(bindKey);
  Serial.println("3");
  Receiver.enableIRIn();
  decode_results results;
  while (!Receiver.decode(&results)) delay(10);
  if (storeRaw){
    Serial.println("RAW");
    EEPROM.write(address,results.rawlen);
    EEPROM.write(address+1,100);// raw type
    EEPROM_writeRAW(getAdrForRaw(bindKey),results.rawbuf,results.rawlen);
  } else {
  Serial.println("4");
  if (results.decode_type == UNKNOWN) return;
  EEPROM.write(address,results.bits); //длина кода в битах
  Serial.println("5");
  EEPROM_writeInt(address+2,results.value); //код
  Serial.println("6");
  EEPROM.write(address+1,results.decode_type); // тип кода, RAW >= 100
  Serial.println("7");
  Serial.println(results.rawlen);
  for (int i=0; i<results.rawlen-1;i++){
    Serial.println(results.rawbuf[i]);
  }
  }
  readEEP();
}


byte getAdr(char button){
  switch(button){
    case '1':return 0;
    case '2':return 6;
    case '3':return 12;
    case '4':return 18;
    case '5':return 24;
    case '6':return 30;
    case '7':return 36;
    case '8':return 42;
    case '9':return 48;
    case '0':return 54;
    case 'A':return 60;
    case 'B':return 66;
    case 'C':return 72;
    case 'D':return 78;
  }
}

byte getAdrForRaw(char button){ //EEPROM can store no more than 3 raw ir codes
  switch(button){
    case '1':return 250;
    case '2':return 500;
    case '3':return 750;
    case '4':return 250;
    case '5':return 500;
    case '6':return 750;
    case '7':return 250;
    case '8':return 500;
    case '9':return 750;
    case '0':return 250;
    case 'A':return 500;
    case 'B':return 750;
    case 'C':return 250;
    case 'D':return 500;
  }
}

void sendCode(char key){
    byte adr = getAdr(key);
    int codeLen = EEPROM.read(adr);
    Serial.println(codeLen);
    int codeType = EEPROM.read(adr+1);
    Serial.println(codeType);
    long unsigned codeValue = EEPROM_readInt(adr+2);
    if (codeType == 100) {
      unsigned int * raw = EEPROM_readRAW(getAdrForRaw(key),codeLen);
      Sender.sendRaw(raw,codeLen,38); //38 khz
      free(raw);
    }
    
    else if (codeType == NEC) {
      Sender.sendNEC(codeValue, codeLen);
      Serial.println(codeValue, HEX);
  } 
  else if (codeType == SONY) {
    Sender.sendSony(codeValue, codeLen);
    Serial.println(codeValue, HEX);
  } 
  else if (codeType == PANASONIC) {
    Sender.sendPanasonic(codeValue, codeLen);
    Serial.println(codeValue, HEX);
  }
  else if (codeType == SAMSUNG) {
    Sender.sendSAMSUNG(codeValue, codeLen);
    Serial.println(codeValue, HEX);
  }
  else if (codeType == JVC) {
    Sender.sendJVC(codeValue, codeLen,0);
    //delayMicroseconds(50);
    //Sender.sendJVC(codeValue, codeLen,1);
    Serial.println(codeValue, HEX);
  }
  else if (codeType == RC5 || codeType == RC6) {
      // Flip the toggle bit for a new button press
      toggle = 1 - toggle;
    
    // Put the toggle bit into the code to send
    codeValue = codeValue & ~(1 << (codeLen - 1));
    codeValue = codeValue | (toggle << (codeLen - 1));
    if (codeType == RC5) {
      Serial.println(codeValue, HEX);
      Sender.sendRC5(codeValue, codeLen);
    } 
    else {
      Sender.sendRC6(codeValue, codeLen);
      Serial.println(codeValue, HEX);
    }
  } 
}

void pcControlStoreCodes(){
  for(byte i=0;i<11;i++){
    decode_results results;
    Receiver.enableIRIn();
    while (!Receiver.decode(&results)) delay(10);
    //EEPROM.write(PC_CONTROL+i*4,results.bits);
    Serial.println("stored");
    EEPROM_writeInt(PC_CONTROL+i*4,results.value);
    delay(500);
  }
}

byte findCode(long unsigned code){
  for(byte i=0;i<11;i++){
  long unsigned storedCode = EEPROM_readInt(PC_CONTROL+i*4);
  if (code == storedCode) return i;
  }
  return 255;
}


void pcControlMode(){
  while(true){
  Receiver.enableIRIn();
  decode_results results;
  Receiver.enableIRIn();
  while (!Receiver.decode(&results)) delay(10);
  byte option = findCode(results.value);
  char scancode;
  bool noMod = false;
  switch (option){
    case 0:scancode = KEY_RIGHT_ARROW;break;
    case 1:scancode = KEY_LEFT_ARROW;break;
    case 2:{
      scancode = 128; //vol up
      noMod = true;
      break;
    }
    case 3:{
      scancode = 129; //vol down
      noMod = true;
      break;
    }
    case 4:Mouse.move(0,-10,0);break;
    case 5:Mouse.move(0,10,0);break;
    case 6:Mouse.move(-10,0,0);break;
    case 7:Mouse.move(10,0,0);break;
    case 8:Mouse.click();break;
    case 9:return;break;
    case 255:scancode = "X";break; //vol down
  }
  Keyboard.writeMod(scancode,noMod);
  //delay(150);
  }
}

