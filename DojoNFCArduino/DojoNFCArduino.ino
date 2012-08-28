#include <MemoryFree.h>



#include <Trestle.h>
#include <Adafruit_PN532.h>
#include <SerialTrestle.h>


#define SCK  (2)
#define MOSI (3)
#define SS   (4)
#define MISO (5)

Adafruit_PN532 nfc(SCK, MISO, MOSI, SS);


//WiFly wifly;
SerialTrestle bridge("DojoStation", "192.168.1.18", 3000, &Serial);
//WiFlyHQTrestle bridge("DojoStation", "192.168.1.18", 3000, &wifly);
//SoftwareSerial debugSerial(9,8);


///* Change these to match your WiFi network */
//const char mySSID[] = "SuzyQ";
//const char myPassword[] = "cr3m3f1ll3d";

long lastTime = 0;

void setup()
{
  Serial.begin(9600);
  Serial.println(freeMemory());
  initializeStationAndSensor();
  Serial.println(freeMemory());
  


  nfc.begin();

  uint32_t versiondata = nfc.getFirmwareVersion();
  if (! versiondata) {
    Serial.print("Didn't find PN53x board");
    while (1); // halt
  }
  // Got ok data, print it out!
  Serial.print("Found chip PN5");
  // configure board to read RFID tags
  nfc.SAMConfig();

  Serial.println("Waiting for an ISO14443A Card ...");
}

void initializeStationAndSensor(){
  //Register the station with an identifier, a name, a description, and whether old data should be overriden.
  //This will create a web page for this station.
  int response = bridge.registerStation("Dojo", "", false);
  if(response < 0){
    Serial.println("Error Creating Station");
  }

  response = bridge.registerAction("StartTurn", "Start", "", startTurn);
  if(response < 0){
    Serial.println("E");
  }
  response = bridge.registerAction("BlackWins", "Black Won", " ", blackWins);
  if(response < 0){
    Serial.println("Er");
  }
  response = bridge.registerAction("WhiteWins", "WhiteWon", " ", whiteWins);
  if(response < 0){
    Serial.println("Error registering Action");
  }
  bridge.addState("BlackPlayer", "Black Card", "Most Recent Card");
  bridge.addState("WhitePlayer", "White Card", "Most Recent Card");

}

void startTurn(char* message){
  //This is called whenever the user clicks on the button on the website.
  Serial.println("Starting a new turn!");
  bridge.setStateValue("WhitePlayer", "");
  bridge.setStateValue("BlackPlayer","");
}

void whiteWins(char* message){
  //This is called whenever the user clicks on the button on the website.
  Serial.println("WhiteWon!");
}
void blackWins(char* message){
  //This is called whenever the user clicks on the button on the website.
  Serial.println("BlackWins");
}
boolean state = false;
void loop()
{
  uint8_t success;
  uint8_t uid[] = { 
    0, 0, 0, 0, 0, 0, 0   };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  if(success){
    //We found a card
    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ... 

      // Now we need to try to authenticate it for read/write access
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF       };

      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);

      if (success)
      {
        //We're authenticated
        uint8_t data[16];
        success = nfc.mifareclassic_ReadDataBlock (4, data);
        
        if(char(data[0]) != 'd' || char(data[1]) != 'o' || char(data[2]) != 'j' || char(data[3]) != 'o'){
          Serial.println("Not a Dojo Card");
         return; 
        }
        //Now we have a dojo card.
        char card[12];
        for(int i=4; i<15; i++){
         card[i-4] = data[i];  
        }
        card[11] = '\0';
        if(char(data[15])=='w'){
         //We have a white player
        bridge.setStateValue("WhitePlayer", (char*)card); 
        }
        if(char(data[15])=='b'){
         //Black player
        bridge.setStateValue("BlackPlayer",(char*)card); 
        }
        
        

//        for(int q; q < 16; q++){
//          Serial.print(char(data[q]));
//          Serial.print("-");
//        }
//        Serial.println();
//        Serial.println("====");

        delay(5000);
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }

    }
  }
//
  if((millis()-lastTime) > 5000){
    bridge.tick();
    lastTime = millis();
  }
//    //Send sensor data to the web page by passing the Station identifier, the sensor identifier, and int version of the value, and the nu
//    //ber to divide by to return to a float.
//      if(state){
//      bridge.setStateValue("BlackPlayer", "Stab");
//      bridge.setStateValue("WhitePlayer", "Card0");
//
//      state = false;
//    } 
//    else{
//      bridge.setStateValue("BlackPlayer", "Card0");
//      bridge.setStateValue("WhitePlayer", "Card1");
//      state = true;
//    }
//  }

}


//Setup the wifi connection.
//void setupWiFi(){
//      if (!wifly.begin(&Serial, &debugSerial)) {
//      Serial.println("WiFly Connection Failed");
//      while(1){
//      }
//    }
//
//    /* Join wifi network if not already associated */
//    if (!wifly.isAssociated()) {
//	/* Setup the WiFly to connect to a wifi network */
//	wifly.setSSID(mySSID);
//	wifly.setPassphrase(myPassword);
//	wifly.enableDHCP();
//
//	if (wifly.join()) {
//	    debugSerial.println("Joined wifi network");
//	} else {
//	    debugSerial.println("Failed to join wifi network");
//          while(1){
//          }
//	}
//    } else {
//        debugSerial.println("Already joined network");
//    }
//
//    wifly.setDeviceID("Wifly-WebClient");
//
//
//    if (wifly.isConnected()) {
//	wifly.close();
//    }
//}


