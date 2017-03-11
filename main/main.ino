#include <IRLib.h>
#include <Keypad.h>
IRrecv My_Receiver(7);
IRsend My_Sender;
IRdecode My_Decoder;

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
  //Serial.begin(9600);
  //My_Receiver.enableIRIn(); // Start the receiver
}
 
void loop() {
//Continuously look for results. When you have them pass them to the decoder
  /*if (My_Receiver.GetResults(&My_Decoder)) {
    My_Decoder.decode();    //Decode the data
    My_Decoder.DumpResults(); //Show the results on serial monitor
    My_Receiver.resume();     //Restart the receiver
    
  }*/
  char customKey = keypad.getKey();
  
  if (customKey){
    //Serial.println(customKey);
    switch (customKey){
      case '1': My_Sender.send(NECX,0xE0E048B7,32);break; //ch+
      case '2': My_Sender.send(NECX,0xE0E008F7,32);break; //ch-
      case '3': My_Sender.send(NECX,0xE0E0E01F,32);break; //vol+
      case 'A': My_Sender.send(NECX,0xE0E0D02F,32);break; //vol-
      case '4': My_Sender.send(NECX,0xE0E040BF,32);break; //pwr
      default:break;
    }
  }
  delay(10);
}
