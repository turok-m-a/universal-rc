//#include <IRLib.h>
#include <EEPROM.h>
#include <IRremote.h>
#include <Keypad.h>
IRrecv Receiver(7);
IRsend Sender;
//decode_results results;
byte holdKeys=0;

byte getAdr(char);
const byte rows = 2; //four rows
const byte cols = 4; //three columns
char keys[rows][cols] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  
};
byte rowPins[rows] = {4, 5 }; //connect to the row pinouts of the keypad
byte colPins[cols] = {12, 11, 10, 8}; //connect to the column pinouts of the keypad
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, rows, cols );
 
void setup()
{
  Receiver.enableIRIn();
  Serial.begin(9600);
  //My_Receiver.enableIRIn(); // Start the receiver
}


  

void loop() {
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
                      holdKeys++; //del
                      if (keypad.key[i].kchar == 'A'){
                        for(int z=0;z<LIST_MAX;z++){
                          if((keypad.key[z].kchar == 'B') && (keypad.key[z].kstate == HOLD))
                          {
                              Serial.println("\nB+A");
                              storeCode();
                          }
                        }
                      }
                      switch (keypad.key[i].kchar){
                           case '1': Sender.sendNEC(0xE0E048B7,32);break; //ch+
                           case '2': Sender.sendNEC(0xE0E008F7,32);break; //ch-
                           case '3': Sender.sendNEC(0xE0E0E01F,32);break; //vol+
                           case 'A': Sender.sendNEC(0xE0E0D02F,32);break; //vol-
                           case '4': Sender.sendNEC(0xE0E040BF,32);break; //pwr
                           default:break;
                      }
                    }
                break;
                    case HOLD:
                    msg = " HOLD.";
                break;
                    case RELEASED:{
                    holdKeys--;
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
      Serial.print("to");
      Serial.println(adr);
      p++;
      adr++;
      
   }
       
}

void readEEP(){
  for(int address=0;address < EEPROM.length();address++){
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




