/* Detects patterns of knocks and triggers a motor to unlock
   it if the pattern is correct.
   
   By Steve Hoefer http://grathio.com
   Version 0.1.10.20.10
   Licensed under Creative Commons Attribution-Noncommercial-Share Alike 3.0
   http://creativecommons.org/licenses/by-nc-sa/3.0/us/
   (In short: Do what you want, just be sure to include this line and the four above it, and don't sell it or use it in anything you sell without contacting me.)
 */
 
#include "pitches.h"

// Tuning constants.  Could be made vars and hoooked to potentiometers for soft configuration, etc.
const int piezoSensor = A0;         // Piezo sensor on pin 0.
const int threshold = 1000;           // Minimum signal from the piezo to register as a knock
const int rejectValue = 35;        // If an individual knock is off by this percentage of a knock we don't unlock..
const int averageRejectValue = 25; // If the average timing of the knocks is off by this percent we don't unlock.
const int knockFadeTime = 150;     // milliseconds we allow a knock to fade before we listen for another one. (Debounce timer.)
const int maximumKnocks = 20;       // Maximum number of knocks to listen for.
const int knockComplete = 1000;     // Longest time to wait for a knock before we assume that it's finished.

// Variables.
int secretCode[maximumKnocks] = {50, 25, 25, 50, 100, 50, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};  // Initial setup: "Shave and a Hair Cut, two bits."
int knockReadings[maximumKnocks];   // When someone knocks this array fills with delays between knocks.
int piezoSensorValue = 0;           // Last reading of the knock sensor.
boolean enabled = true;


void setupKnock() {
  pinMode(piezoSensor, INPUT_PULLUP);
  delay(50);
}

void loopKnock() {
  if(!enabled) return;
  
  // Listen for any knock at all.
  piezoSensorValue = analogRead(piezoSensor);
  if (piezoSensorValue <= threshold){
    listenToSecretKnock();
  }
} 

// Records the timing of knocks.
void listenToSecretKnock(){
  Serial.println("knock...");
  int i = 0;
  // First lets reset the listening array.
  for (i=0;i<maximumKnocks;i++){
    knockReadings[i]=0;
  }
  
  int currentKnockNumber=0;         			// Incrementer for the array.
  int startTime=millis();           			// Reference for when this knock started.
  int now=millis();
  delay(knockFadeTime);
  
  do {
    //listen for the next knock or wait for it to timeout. 
    pinMode(piezoSensor, INPUT_PULLUP);
    piezoSensorValue = analogRead(piezoSensor);
    if (piezoSensorValue <= threshold){                   //got another knock...
      Serial.println("knock...");
      now=millis();
      knockReadings[currentKnockNumber] = now-startTime;
      currentKnockNumber ++;                             //increment the counter
      startTime=now;
      delay(knockFadeTime);
    }
   
    now=millis();
    
    //did we timeout or run out of knocks?
  } while ((now-startTime < knockComplete) && (currentKnockNumber < maximumKnocks));
  
  //we've got our knock recorded, lets see if it's valid
  if (validateKnock() == true){
    Serial.println("Yeaayy !");
    enabled = false;
    pinMode(piezo, OUTPUT);
    tone(piezo, 400);
    delay(50);
    tone(piezo, 600);
    delay(50);
    tone(piezo, 800);
    delay(50);
    tone(piezo, 1000);
    delay(250);
    noTone(piezo);
    enablePiano();
  } else {
    Serial.println("Fail...");
    pinMode(piezoSensor, OUTPUT);
    tone(piezoSensor, 300, 20);
    delay(50);
    tone(piezoSensor, 300, 20);
    delay(50);
    tone(piezoSensor, 300, 20);
    delay(50);
    pinMode(piezoSensor, INPUT_PULLUP);
    delay(50);
  }
}

// Sees if our knock matches the secret.
// returns true if it's a good knock, false if it's not.
// todo: break it into smaller functions for readability.
boolean validateKnock(){
  int i=0;
 
  // simplest check first: Did we get the right number of knocks?
  int currentKnockCount = 0;
  int secretKnockCount = 0;
  int maxKnockInterval = 0;          			// We use this later to normalize the times.
  
  for (i=0;i<maximumKnocks;i++){
    if (knockReadings[i] > 0){
      currentKnockCount++;
    }
    if (secretCode[i] > 0){  					//todo: precalculate this.
      secretKnockCount++;
    }
    
    if (knockReadings[i] > maxKnockInterval){ 	// collect normalization data while we're looping.
      maxKnockInterval = knockReadings[i];
    }
  }

  
  if (currentKnockCount != secretKnockCount){
    return false; 
  }
  
  /*  Now we compare the relative intervals of our knocks, not the absolute time between them.
      (ie: if you do the same pattern slow or fast it should still open the door.)
      This makes it less picky, which while making it less secure can also make it
      less of a pain to use if you're tempo is a little slow or fast. 
  */
  int totaltimeDifferences=0;
  int timeDiff=0;
  for (i=0;i<maximumKnocks;i++){ // Normalize the times
    knockReadings[i]= map(knockReadings[i],0, maxKnockInterval, 0, 100);      
    timeDiff = abs(knockReadings[i]-secretCode[i]);
    if (timeDiff > rejectValue){ // Individual value too far out of whack
      return false;
    }
    totaltimeDifferences += timeDiff;
  }
  // It can also fail if the whole thing is too inaccurate.
  if (totaltimeDifferences/secretKnockCount>averageRejectValue){
    return false; 
  }
  
  return true;
  
}
