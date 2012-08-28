/**************************************************************************/
/*! 
    @file     readMifare.pde
    @author   Adafruit Industries
	@license  BSD (see license.txt)

    This example will wait for any ISO14443A card or tag, and
    depending on the size of the UID will attempt to read from it.
   
    If the card has a 4-byte UID it is probably a Mifare
    Classic card, and the following steps are taken:
   
    - Authenticate block 4 (the first block of Sector 1) using
      the default KEYA of 0XFF 0XFF 0XFF 0XFF 0XFF 0XFF
    - If authentication succeeds, we can then read any of the
      4 blocks in that sector (though only block 4 is read here)
	 
    If the card has a 7-byte UID it is probably a Mifare
    Ultralight card, and the 4 byte pages can be read directly.
    Page 4 is read by default since this is the first 'general-
    purpose' page on the tags.


This is an example sketch for the Adafruit PN532 NFC/RFID breakout boards
This library works with the Adafruit NFC breakout 
  ----> https://www.adafruit.com/products/364
 
Check out the links above for our tutorials and wiring diagrams 
These chips use SPI to communicate, 4 required to interface

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

*/
/**************************************************************************/

#include <Adafruit_PN532.h>

#define SCK  (2)
#define MOSI (3)
#define SS   (4)
#define MISO (5)

Adafruit_PN532 nfc(SCK, MISO, MOSI, SS);

int index;
uint8_t card_data[6][16] = {
  { 'd', 'o', 'j', 'o', 'o', 'v', 'e', 'r', 'h', 'e', 'a', 'd', 'c', 'u', 't', 0},
  { 'd', 'o', 'j', 'o', 'e', 'v', 'a', 'd', 'e', 0, 0, 0, 0, 0, 0, 0},
  { 'd', 'o', 'j', 'o', 'p', 'a', 'r', 'r', 'y', 0, 0, 0, 0, 0, 0, 0},
  { 'd', 'o', 'j', 'o', 's', 't', 'a', 'b', 0, 0, 0, 0, 0, 0, 0, 0},
  { 'd', 'o', 'j', 'o', 's', 'i', 'd', 'e', 'c', 'u', 't', 0, 0, 0, 0, 0},
  { 'd', 'o', 'j', 'o', 'c', 'h', 'a', 'r', 'g', 'e', 0, 0, 0, 0, 0, 0}
};

int specials;
uint8_t special_data[6][16] = {
  {'d','o','j','o','a','d','r','e','n','a','l','i','n','e',0,0},
  {'d','o','j','o','e','l','b','o','w','s','h','o','t',0,0,0},
  {'d','o','j','o','s','u','p','e','r','s','t','a','b',0,0,0},
  {'d','o','j','o','c','l','o','s','e','t','h','e','g','a','p', 0},
  {'d','o','j','o','f','o','c','u','s',0,0,0,0,0,0,0},
  {'d','o','j','o','f','e','r','o','c','i','t','y',0,0,0,0}
};

void setup(void) {
  Serial.begin(9600);
  Serial.println("Hello!");
  Serial.println("Hello world!");
  index = 0;
  
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


void loop(void) {
  uint8_t success;
  uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
  uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
  
  // Wait for an ISO14443A type cards (Mifare, etc.).  When one is found
  // 'uid' will be populated with the UID, and uidLength will indicate
  // if the uid is 4 bytes (Mifare Classic) or 7 bytes (Mifare Ultralight)
  success = nfc.readPassiveTargetID(PN532_MIFARE_ISO14443A, uid, &uidLength);
  
  if (success) {
    // Display some basic information about the card
    Serial.println("Found an ISO14443A card");
    Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
    Serial.print("  UID Value: ");
    nfc.PrintHex(uid, uidLength);
    Serial.println("");
    
    if (uidLength == 4)
    {
      // We probably have a Mifare Classic card ... 
      Serial.println("Seems to be a Mifare Classic card (4 byte UID)");
      
      // Now we need to try to authenticate it for read/write access
      // Try with the factory default KeyA: 0xFF 0xFF 0xFF 0xFF 0xFF 0xFF
      Serial.println("Trying to authenticate block 4 with default KEYA value");
      uint8_t keya[6] = { 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF };
	  
	  // Start with block 4 (the first block of sector 1) since sector 0
	  // contains the manufacturer data and it's probably better just
	  // to leave it alone unless you know what you're doing
      success = nfc.mifareclassic_AuthenticateBlock(uid, uidLength, 4, 0, keya);
	  
      if (success)
      {
        uint8_t data[16];
        int cur_index = index % 9;
        Serial.println("Sector 1 (Blocks 4..7) has been authenticated");
        Serial.println("=======");
        Serial.println("=======");
        Serial.println("We are on index: ");
        Serial.println(index);
        Serial.println("The card value is: ");
        if( cur_index < 6){
          for(int i=0; i<16; i++){
            Serial.print(char(card_data[cur_index][i]));
            data[i] = char(card_data[cur_index][i]);
            Serial.print("-");
          }
        }else{
          Serial.println("SPECIAL CARD CATCH");
          int start   = ( int(index / 9) == 0 ) ? 0 : 3;
          int special = (start+(cur_index%3));
          
          for(int i=0; i<16; i++){
            Serial.print(char(special_data[special][i]));
            Serial.print("-");
            data[i] = special_data[special][i]; 
          }
        }
        Serial.println();
        Serial.println("The card color is: ");
        if( int(index / 9) == 0 ){
          Serial.println("white");
          data[15] = 'w';
        } else {
          Serial.println("black ");
          data[15] = 'b';
        }
        Serial.println();
        Serial.println("=======");
        Serial.println("=======");
        index ++;
        
        
        if( index == 18){
          index = 0;
        }
        
        // If you want to write something to block 4 to test with, uncomment
	// the following line and this text should be read back in a minute
        //uint8_t data[16] = { 'a', 'd', 'a', 'f', 'r', 'u', 'i', 't', '.', 'c', 'o', 'm', 0, 0, 0, 0};
        success = nfc.mifareclassic_WriteDataBlock (4, data);
        delay(5000);
      }
      else
      {
        Serial.println("Ooops ... authentication failed: Try another key?");
      }
    }
    
  }
}

