//#include <IRLib.h>
#include <EEPROM.h>
#include <IRremote.h>
#include <Keypad.h>
IRrecv Receiver(7);
IRsend Sender;
//decode_results results;
void sendCode(char);
byte getAdr(char);
byte toggle = 0;
const byte rows = 4; //four rows
const byte cols = 4; //three columns
char keys[rows][cols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
  
};
byte rowPins[rows] = {2, 10, 9, 6}; //connect to the row pinouts of the keypad
byte colPins[cols] = {8, 5, 4, 3}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );

void setup()
{
  Receiver.enableIRIn();
  Serial.begin(9600);
  Serial1.begin(9600);
  //My_Receiver.enableIRIn(); // Start the receiver
}




void loop() {
  if(Serial1.available()){
    Serial.write(Serial1.read());
    Serial1.write("+++\n");
  }
//Continuously look for results. When you have them pass them to the decoder
  /*if (My_Receiver.GetResults(&My_Decoder)) {
    My_Decoder.decode();    //Decode the data
    My_Decoder.DumpResults(); //Show the results on serial monitor
    My_Receiver.resume();     //Restart the receiver
    
  }*/
  //char customKey = keypad.getKey();
  
  //if (customKey){
    //Serial.println(customKey);
    
  //}
  delay(10);
char * msg;
  if (keypad.getKeys())
    {
        for (int i=0; i<LIST_MAX; i++)   // Scan the whole key list.
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
                              storeCode();
                          }
                        }
                      }
                      if (keypad.key[i].kchar == '*'){
                        for(int z=0;z<LIST_MAX;z++){
                          if((keypad.key[z].kchar == '#') && (keypad.key[z].kstate == HOLD))
                          {
                              Serial.println("\n*+#");
                              digitalWrite(13,HIGH);
                              delay(15000);
                          }
                        }
                      }
                      sendCode(keypad.key[i].kchar);
                    }
                break;
                    case HOLD:
                    msg = " HOLD.";
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

void storeCode(){
  Serial.println("1");
  char bindKey = keypad.waitForKey();
  Serial.println("2");
  byte address = getAdr(bindKey);
  Serial.println("3");
  Receiver.enableIRIn();
  decode_results results;
  while (!Receiver.decode(&results)) delay(10);
  Serial.println("4");
  if (results.decode_type == UNKNOWN) return;
  EEPROM.write(address,results.bits);
  Serial.println("5");
  EEPROM_writeInt(address+2,results.value);
  Serial.println("6");
  EEPROM.write(address+1,results.decode_type);
  Serial.println("7");
  Serial.println(results.rawlen);
  for (int i=0; i<results.rawlen-1;i++){
    Serial.println(results.rawbuf[i]);
  }
  readEEP();
}


byte getAdr(char button){
  switch(button){
    case '1':return 0;
    case '2':return 10;
    case '3':return 20;
    case '4':return 30;
    case '5':return 40;
    case '6':return 50;
    case '7':return 60;
    case '8':return 70;
    case '9':return 80;
    case '0':return 90;
    case 'A':return 100;
    case 'B':return 110;
    case 'C':return 120;
    case 'D':return 130;
  }
}

void sendCode(char key){
    byte adr = getAdr(key);
    int codeLen = EEPROM.read(adr);
    Serial.println(codeLen);
    int codeType = EEPROM.read(adr+1);
    Serial.println(codeType);
    long unsigned codeValue = EEPROM_readInt(adr+2);
    if (codeType == NEC) {
      Sender.sendNEC(codeValue, codeLen);
      Serial.print("Sent NEC ");
      Serial.println(codeValue, HEX);
  } 
  else if (codeType == SONY) {
    Sender.sendSony(codeValue, codeLen);
    Serial.print("Sent Sony ");
    Serial.println(codeValue, HEX);
  } 
  else if (codeType == PANASONIC) {
    Sender.sendPanasonic(codeValue, codeLen);
    Serial.print("Sent Panasonic");
    Serial.println(codeValue, HEX);
  }
  else if (codeType == SAMSUNG) {
    Sender.sendSAMSUNG(codeValue, codeLen);
    Serial.print("Sent samsung ");
    Serial.println(codeValue, HEX);
  }
  else if (codeType == JVC) {
    Sender.sendJVC(codeValue, codeLen,0);
    delayMicroseconds(50);
    Sender.sendJVC(codeValue, codeLen,1);
    Serial.print("Sent JVC");
    Serial.println(codeValue, HEX);
  }
  else if (codeType == RC5 || codeType == RC6) {
      // Flip the toggle bit for a new button press
      toggle = 1 - toggle;
    
    // Put the toggle bit into the code to send
    codeValue = codeValue & ~(1 << (codeLen - 1));
    codeValue = codeValue | (toggle << (codeLen - 1));
    if (codeType == RC5) {
      Serial.print("Sent RC5 ");
      Serial.println(codeValue, HEX);
      Sender.sendRC5(codeValue, codeLen);
    } 
    else {
      Sender.sendRC6(codeValue, codeLen);
      Serial.print("Sent RC6 ");
      Serial.println(codeValue, HEX);
    }
  } 
}

