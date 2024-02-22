// Description: 
//    This code will initialize EEPROM values to get a clean Buzz Wire high score list with "NoScore" and 0 time.
//    It may also be used to re-write earlier high scores if needed. 
//    Note that name must be 8 chars and time given in milliseconds.
//
// Usage:
//    Simple uplode this code to your board, and it will run and write EEPROM. Then go and upload the main Buzz Wire code.
//
// Author: 
//    The Skjegg (Aka Torbjørn Skjeggestad) 22/02/2024
//
// V000: First version

#include <EEPROM.h>

#define H1 10
#define H2 30
#define H3 50
#define H4 70
#define H5 90
#define H6 110
#define H7 130
#define H8 150
#define H9 170
#define H10 190

void setup() {
/*
  // Example of re-writing an old list:
  EEPROMWriteScore(H1, "Doc     ", 24178);
  EEPROMWriteScore(H2, "Marty   ", 25715);
  EEPROMWriteScore(H3, "Jennifer", 35764);
  EEPROMWriteScore(H4, "Baines  ", 37205);
  EEPROMWriteScore(H5, "G.McFly ", 49467);
  EEPROMWriteScore(H6, "Babs    ", 73877);
  EEPROMWriteScore(H7, "Dave    ", 83527);
  EEPROMWriteScore(H8, "Stella  ", 115896);
  EEPROMWriteScore(H9, "Biff    ", 124345);
*/

  EEPROMWriteScore(H1, "NoScore ", 0);
  EEPROMWriteScore(H2, "NoScore ", 0);
  EEPROMWriteScore(H3, "NoScore ", 0);
  EEPROMWriteScore(H4, "NoScore ", 0);
  EEPROMWriteScore(H5, "NoScore ", 0);
  EEPROMWriteScore(H6, "NoScore ", 0);
  EEPROMWriteScore(H7, "NoScore ", 0);
  EEPROMWriteScore(H8, "NoScore ", 0);
  EEPROMWriteScore(H9, "NoScore ", 0);
}

void EEPROMWriteScore(int EepromAdr, char *Name, long Score){ // Skriv navn og score
  int n=0;
  for(int i=EepromAdr ; i < (EepromAdr+8) ; i++){ // Skriv navn
    EEPROM.write(i,Name[n]);
    n++;
  }
  EEPROMWriteLong((EepromAdr+10), Score); // Skriv score
}

void EEPROMWriteLong(int EepromAdr, long value){ // Skriver en long verdi til EEPROM
      //Decomposition from a long to 4 bytes by using bitshift.
      //One = Most significant -> Four = Least significant byte
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      //Write the 4 bytes into the eeprom memory.
      EEPROM.write(EepromAdr, four);
      EEPROM.write(EepromAdr + 1, three);
      EEPROM.write(EepromAdr + 2, two);
      EEPROM.write(EepromAdr + 3, one);
}

void loop() {
  // put your main code here, to run repeatedly:

}
