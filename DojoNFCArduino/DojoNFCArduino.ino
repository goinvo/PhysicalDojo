#include <MemoryFree.h>



#include <Trestle.h>
#include <SerialTrestle.h>

SerialTrestle bridge("DojoStation", "192.168.1.18", 3000, &Serial);

long lastTime = 0;

void setup()
{
    Serial.begin(9600);
    Serial.println(freeMemory());
    initializeStationAndSensor();
    Serial.println(freeMemory());

}

void initializeStationAndSensor(){
    //Register the station with an identifier, a name, a description, and whether old data should be overriden.
   //This will create a web page for this station.
   int response = bridge.registerStation("Dojo The Game", "The Physical Version", false);
   if(response < 0){
    Serial.println("Error Creating Station");
   }

   response = bridge.registerAction("StartTurn", "Start A New Turn", "Starting a New Turn", startTurn);
   if(response < 0){
      Serial.println("Error registering Action");
   }
   response = bridge.registerAction("BlackWins", "Black Won", " ", blackWins);
   if(response < 0){
      Serial.println("Error registering Action");
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
  if((millis()-lastTime) > 5000){
    bridge.tick();
    lastTime = millis();
    //Send sensor data to the web page by passing the Station identifier, the sensor identifier, and int version of the value, and the number to divide by to return to a float.
    if(state){
      bridge.setStateValue("BlackPlayer", "Card1");
      bridge.setStateValue("WhitePlayer", "Card0");

      state = false;
    } else{
      bridge.setStateValue("BlackPlayer", "Card0");
      bridge.setStateValue("WhitePlayer", "Card1");
      state = true;
    }
  }

}


