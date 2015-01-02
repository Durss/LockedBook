/**
  Melody
 
 Plays a melody 
 
 circuit:
 * 8-ohm speaker on digital pin 8
 
 created 21 Jan 2010
 modified 30 Aug 2011
 by Tom Igoe 

This example code is in the public domain.
 
 http://arduino.cc/en/Tutorial/Tone
 
 */
#include "pitches.h"
#include <EEPROM.h>
 
char song[] = "gabygabyxzCDxzCDabywabywzCDEzCDEbywFCDEqywFGDEqi        azbC"; // a space represents a rest
int length = sizeof(song); // the number of notes
int beats[] = { 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, 2,3,3,16,};
int tempo = 75;
int notes[] = {NOTE_C4, NOTE_D4, NOTE_E4, NOTE_G4, NOTE_C5, NOTE_D5};
int unlockSequence[] = {2,3,1,0,1,2,3,1,2,3,5,4,3};
int unlockSequencePointer = 0;
int lockSequence[] = {2,1,0};
int lockSequencePointer = 0;
int numNotes  = sizeof(notes)/sizeof(int);
unsigned long lastMs;

int softpotPin = A1;
int value;
int prevValue;
int lastV;
int lastTestedV;
int speakerPin = A0;
boolean codeFound = false;
boolean pianoEnabled = false;
boolean closeBookOnRelease = false;

void setupPiano() {
  pinMode(softpotPin, INPUT_PULLUP);
}

void enablePiano() {
  pianoEnabled = true;
  pinMode(speakerPin, OUTPUT);
  lastTestedV = -1;
}

int address = 0;
void loopPiano() {
  if(!pianoEnabled) return;
  
  value = analogRead(softpotPin);
  int v;
  //Warning ! Values are wrong when plugged to USB
  //there is a kind of huge offset where ~1cm of the
  //softpot stays to 0 at the left.
  if(value<=219) v =  0;
  else if(value<=400) v =  1;
  else if(value<=540) v =  2;
  else if(value<=695) v =  3;
  else if(value<=845) v =  4;
  else if(value<=1000) v = 5;
  if(value >= 1000 && closeBookOnRelease) {
    closeBook();
    closeBookOnRelease = false;
  }
  
  if(codeFound && v >= numNotes) {
    openBook();
    delay(250);
    pinMode(speakerPin, OUTPUT);
    //Play success music
    for (int i = 0; i < length; i++) {
      if (song[i] == ' ') {
        delay(beats[i] * tempo); // rest
      } else {
        playNote(song[i], beats[i] * tempo);
      }
      
      // pause between notes
      delay(tempo / 2);
    }
    delay(100);
    codeFound = false;
    unlockSequencePointer = 0;
    return;
  }
  if(v != lastV) {
    lastV = v;
    lastMs = millis();
  }
  if(millis() - lastMs < 50) return;
  
  if( v < numNotes) {
    int n = notes[v];
    pinMode(speakerPin, OUTPUT);
    tone(speakerPin, n, 10);
    if(v != lastTestedV) {
      lastTestedV = v;
      if(unlockSequence[unlockSequencePointer] == v) unlockSequencePointer ++;
      else unlockSequencePointer = 0;
      if(unlockSequencePointer == sizeof(unlockSequence)/sizeof(int)) codeFound = true;
      
      if(lockSequence[lockSequencePointer] == v) lockSequencePointer ++;
      else lockSequencePointer = 0;
      if(lockSequencePointer == sizeof(lockSequence)/sizeof(int)) {
        lockSequencePointer = 0;
        closeBookOnRelease = true;
      }
    }
  }
  return;
}


void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}


void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'x', 'a', 'z', 'b', 'C', 'y', 'D', 'w', 'E', 'F', 'q', 'G', 'i' };
  // c=C4, C = C5. These values have been tuned.
  int tones[] = { 1898, 1690, 1500, 1420, 1265, 1194, 1126, 1063, 1001, 947, 893, 843, 795, 749, 710, 668, 630, 594 };
   
  // play the tone corresponding to the note name
  for (int i = 0; i < 18; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}
