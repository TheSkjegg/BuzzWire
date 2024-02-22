// Description: 
//    Buzz Wire game with timer and high score list. Start/stop pads ensure that timing starts and stops, enabling a precise timing
//    of each game.
//    Remember to load ResetEEPROM code first, to initialize high score list.
//
// Usage:
//    Developed on the Adafruit Metro Mini board, but should be compatible with all "Arduino UNO" boards.
//    Simply upload this code to your board and test your Buzz Wire game!
//
// Author: 
//    The Skjegg (Aka Torbjørn Skjeggestad) 22/02/2024
//
// Versions:
//    V000-032: Not published dev
//    V033: First public version

#include "LedControl.h"
#include "EEPROM.h"
#include "U8g2lib.h"

U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

// IO def
#define CLK 8 // Rotary Encoder
#define DT A0 // Rotary Encoder
#define SW 4 // Rotary Encoder
#define StartPad1 5
#define StartPad2 6
#define WireIO 3 // Interrupt
#define Buzz 9 // Buzzer
#define Volt A1
#define Led1 A2
#define Led2 A3
#define Led3 13

LedControl lc=LedControl(12,11,10,1);
// 12 --> DIN, 11 --> CLK, 10 --> CS

// Globale vars
byte versjon = 33;
bool nono = false;
int counter=0;
int counter_prev=0;
int encoder_A;
int encoder_B;
int encoder_A_prev=0;
char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789!%&/()+-? ";
int arraySize = 0;
char navn[9] = "";
byte nameSize = 0;
bool OK = false; 
bool Confirmed = false;
byte flashCount = 0;
bool flash = false;
bool mute = false;

byte showHighScoreNo = 0;
int batSaver = 0;

// Voltage
float volt = 0;
float sum = 0;
int ant_maalinger = 5;

// Timing
unsigned long Start, Elapsed;

byte flashHighScores=0;
boolean newStart = false;
volatile boolean failed = false;
volatile byte failed_no = 0;
byte listePlass=0;

// timeToChar
char Time1[10];
char Time2[10];
char Time3[10];

// EEPROM
char NameRead1[8];
char NameRead2[8];
char NameRead3[8];
long Score=0;
long highScores[10];

void setup() {
  u8g2.begin();

  //IO
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);
  pinMode(SW, INPUT);
  digitalWrite(SW, HIGH); // Pullup
  pinMode(StartPad1, INPUT); // StartPad
  digitalWrite(StartPad1, HIGH); // pullup
  pinMode(StartPad2, INPUT); // StopPad
  digitalWrite(StartPad2, HIGH); // pullup
  pinMode(WireIO, INPUT); // StopPad
  digitalWrite(WireIO, HIGH); // pullup
  pinMode(Led1, OUTPUT);
  digitalWrite(Led1, LOW);
  pinMode(Led2, OUTPUT);
  digitalWrite(Led1, LOW);
  pinMode(Led3, OUTPUT);
  digitalWrite(Led1, LOW);

  arraySize = sizeof(chars) - 2;

  // 7segment
  lc.shutdown(0,true);
  lc.setIntensity(0,1);
  lc.clearDisplay(0);
  displayTime(0);

  // Display Made by screen:
  updateOLED(8);
  successSound();
  delay(4000);

  attachInterrupt(1, busted, CHANGE);
  delay(100);
  detachInterrupt(1);
  failed_no = 0;
  failed = false;
}

void updateOLED(int funk){
    u8g2.firstPage();  
    do {
      if(funk == 1)
        draw_highscore();
      else if (funk == 2)
        draw_start();
      else if (funk == 3)
        draw_ongoing();
      else if (funk == 4)
        draw_finished();
      else if (funk == 5)
        draw_failed();
      else if (funk == 6)
        draw_writeName();
      else if (funk == 7)
        draw_confirmName();
      else if (funk == 8)
        draw_startScreen();
      else if (funk == 9)
        draw_about();
      else if (funk == 10)
        draw_LowBat();
      else if (funk == 11)
        draw_BatSaver();
      else if (funk == 12)
        draw_saveHS();
    } while( u8g2.nextPage() );
}

void ReadHighscores(int StartAdr){ // 10=1-3place, 70=4-6place, 130=7-9place
  int Adr = StartAdr;
  ReadName(Adr, NameRead1);
  Score = ReadScore(Adr);
  timeToChar(Score, Time1);
  Adr = Adr + 20;
  ReadName(Adr, NameRead2);
  Score = ReadScore(Adr);
  timeToChar(Score, Time2);
  Adr = Adr + 20;
  ReadName(Adr, NameRead3);
  Score = ReadScore(Adr);
  timeToChar(Score, Time3);
}

void draw_highscore(void){ // 1
  u8g2.setFont(u8g2_font_timR10_tf);
  u8g2.setCursor(25,10);
  u8g2.print("Highscores");
  u8g2.setFont(u8g2_font_timR08_tf);
  u8g2.setCursor(0,25);
  u8g2.print(showHighScoreNo);
  u8g2.print(": ");
  for(int k=0;k<8;k++)
    u8g2.print(NameRead1[k]);
  u8g2.setCursor(75,25);
  u8g2.print(Time1);
  u8g2.setCursor(0,37);
  u8g2.print(showHighScoreNo + 1);
  u8g2.print(": ");
  for(int k=0;k<8;k++)
    u8g2.print(NameRead2[k]);
  u8g2.setCursor(75,37);
  u8g2.print(Time2);
  u8g2.setCursor(0,49);
  u8g2.print(showHighScoreNo + 2);
  u8g2.print(": ");
  for(int k=0;k<8;k++)
    u8g2.print(NameRead3[k]);
  u8g2.setCursor(75,49);
  u8g2.print(Time3);
}

void draw_start(void){ // 2
  u8g2.setFont(u8g2_font_timR18_tf);
  u8g2.setCursor(15,35);
  u8g2.print("READY!");
}

void draw_ongoing(void){ // 3
  u8g2.setFont(u8g2_font_timR18_tf);
  u8g2.setCursor(10,35);
  u8g2.print("Good luck!");
}

void draw_finished(void){ // 4
  u8g2.setFont(u8g2_font_timR10_tf);
  if(listePlass > 9){
    u8g2.setCursor(25,10);
    u8g2.print("Well done!!");
    u8g2.setCursor(15,35);
    u8g2.print(".. but no record.");
  }
  else{
    u8g2.setCursor(35,10);
    u8g2.print("Fantastic!");
    u8g2.setCursor(42,35);
    u8g2.print("Place ");
    u8g2.print(listePlass);
  }
  
}

void draw_failed(void){ // 5
  u8g2.setFont(u8g2_font_timR18_tf);
  u8g2.setCursor(5,20);
  u8g2.print("Game Over");
}

void draw_writeName(void){ // 6
  u8g2.setFont(u8g2_font_timR10_tf);
  u8g2.setCursor(0,12);
  u8g2.print("Write name:"); 
  u8g2.setCursor(0,37);
  int i=0;
  while(i < nameSize){
    u8g2.print(navn[i]);
    i++;
  }
  if(flash)
    u8g2.print(chars[counter]);

  u8g2.setFont(u8g2_font_timR08_tf);
  u8g2.setCursor(0,60);
  u8g2.print("(");
  u8g2.print(nameSize);
  u8g2.print("/8)");
  
}

void draw_confirmName(void){ // 7
  u8g2.setFont(u8g2_font_timR10_tf);
  u8g2.setCursor(30,12);
  int i=0;
  while(i < nameSize){
    u8g2.print(navn[i]);
    i++;
  }
  u8g2.setCursor(50,35);
  u8g2.print("OK?");
  u8g2.setFont(u8g2_font_timR10_tf);
  u8g2.setCursor(10,55);
  u8g2.print("YES");
  u8g2.setCursor(90,55);
  u8g2.print("NO");
  if(flash){
    if(OK)
      u8g2.drawFrame(8,42,30,15); // x,y,w,h
    else
      u8g2.drawFrame(88,42,25,15);
    }
  }

void draw_startScreen(void){ // 8
  u8g2.setFont(u8g2_font_timR18_tf);
  u8g2.setCursor(10,20);
  u8g2.print("BuzzWire!");
  u8g2.setFont(u8g2_font_timR10_tf);
  u8g2.setCursor(5,45);
  u8g2.print("Made by The Skjegg");
}

void draw_about(void){ // 9
  u8g2.setFont(u8g2_font_timR10_tf);
  u8g2.setCursor(30,12);
  u8g2.print("BuzzWire!");
  u8g2.setFont(u8g2_font_timR08_tf);
  u8g2.setCursor(0,25);
  u8g2.print("Made by: The Skjegg");
  u8g2.setCursor(0,35);
  u8g2.print("SW version: ");
  u8g2.print(versjon);
  u8g2.setCursor(0,45);
  u8g2.print("Volt: ");
  u8g2.print(volt);
  u8g2.setCursor(0,56);
  if(mute)
    u8g2.print("Sound: OFF");
  else
    u8g2.print("Sound: ON");
}

void draw_LowBat(void){ // 10
  u8g2.setFont(u8g2_font_timR10_tf);
  u8g2.setCursor(25,40);
  u8g2.print("Low Battery!");
}

void draw_BatSaver(void){ // 11
  u8g2.setFont(u8g2_font_timR10_tf);
  u8g2.setCursor(25,20);
  u8g2.print("Turn me OFF!");
  u8g2.setCursor(0,40);
  u8g2.print("You dont play with me :(");
}

void draw_saveHS(void){ // 12
  u8g2.setFont(u8g2_font_timR10_tf);
  u8g2.setCursor(10,12);
  u8g2.print("Store highscore?");
  u8g2.setCursor(8, 25);
  u8g2.setFont(u8g2_font_timR08_tf);
  u8g2.print("(Don't if you already got ");
  u8g2.setCursor(8, 35);
  u8g2.print("better than place ");
  u8g2.print(listePlass);
  u8g2.print("!)");
  u8g2.setFont(u8g2_font_timR10_tf);
  u8g2.setCursor(10,55);
  u8g2.print("YES");
  u8g2.setCursor(90,55);
  u8g2.print("NO");
  if(flash){
    if(OK)
      u8g2.drawFrame(8,42,30,15); // x,y,w,h
    else
      u8g2.drawFrame(88,42,25,15);
    }
  }

bool saveHS(){
    OK = true;
    updateOLED(12);
    while(1){
    delay(2);
    
    flashCount++;
    if(flashCount > 250){
      flashCount = 0;
      flash = !flash;
      updateOLED(12);
    }
    
    encoder_A = digitalRead(CLK);
    encoder_B = digitalRead(DT);
    if((!encoder_A) && (encoder_A_prev)){
      OK = !OK;
      tone(Buzz, 150, 5);
      flash = true;
      flashCount = 0;
      updateOLED(12);
    }
    encoder_A_prev = encoder_A;

    if(!digitalRead(SW)){
      delay(200); // debounce
      if(OK){
        OK = false;
        return true;
      }
      else
        return false;
    }
  }
}

void writeName(){

  while(nameSize < 8){
    delay(1);
    flashCount++;

    encoder_A = digitalRead(CLK);
    encoder_B = digitalRead(DT);
    if((!encoder_A) && (encoder_A_prev)){
      if(encoder_B){
        counter++;
        tone(Buzz, 100, 5);
      }
      else {
        counter--;
        tone(Buzz, 150, 5);
      }
    }
    encoder_A_prev = encoder_A;
    if(!(counter == counter_prev)){
      if(counter < 0)
        counter = arraySize;
      else if(counter > arraySize)
        counter = 0;
   
      flash = true;
      flashCount = 0;
      updateOLED(6);
    }
    counter_prev = counter; 
  
    if(flashCount > 250){
      flashCount = 0;
      flash = !flash;
      updateOLED(6);
    }

    if(!digitalRead(SW)){
      navn[nameSize] = chars[counter];
      nameSize++;
      delay(300); // debounce
    }
  } // while end
  
  delay(1000);
  confirmName();

}

void confirmName(){
  OK = true;
  updateOLED(7);
  while(!Confirmed){
    delay(2);
    
    flashCount++;
    if(flashCount > 250){
      flashCount = 0;
      flash = !flash;
      updateOLED(7);
    }
    
    encoder_A = digitalRead(CLK);
    encoder_B = digitalRead(DT);
    if((!encoder_A) && (encoder_A_prev)){
      OK = !OK;
      tone(Buzz, 150, 5);
      flash = true;
      flashCount = 0;
      updateOLED(7);
    }
    encoder_A_prev = encoder_A;

    if(!digitalRead(SW)){
      if(OK){
        Confirmed = true;
      }
      else {
        for(int i=0;i<8;i++)
          navn[i] = ' ';
        nameSize = 0;
        delay(500);
        writeName();
    }
    delay(200); // debounce
    }
  }
  Confirmed = false;
}

void EEPROMWriteScore(int EepromAdr, char *Name, long Score){
  int n=0;
  for(int i=EepromAdr ; i < (EepromAdr+8) ; i++){
    EEPROM.write(i,Name[n]);
    n++;
  }
  EEPROMWriteLong((EepromAdr+10), Score);
}

void EEPROMWriteLong(int EepromAdr, long value){
      byte four = (value & 0xFF);
      byte three = ((value >> 8) & 0xFF);
      byte two = ((value >> 16) & 0xFF);
      byte one = ((value >> 24) & 0xFF);

      EEPROM.write(EepromAdr, four);
      EEPROM.write(EepromAdr + 1, three);
      EEPROM.write(EepromAdr + 2, two);
      EEPROM.write(EepromAdr + 3, one);
}

long EEPROMReadLong(int EepromAdr){
      long four = EEPROM.read(EepromAdr);
      long three = EEPROM.read(EepromAdr + 1);
      long two = EEPROM.read(EepromAdr + 2);
      long one = EEPROM.read(EepromAdr + 3);

      return ((four << 0) & 0xFF) + ((three << 8) & 0xFFFF) + ((two << 16) & 0xFFFFFF) + ((one << 24) & 0xFFFFFFFF);
}

void ReadName (int EepromAdr, char *ReadName){
  int n=0;
  for(int i=EepromAdr ; i < (EepromAdr+8) ; i++){
    ReadName[n] = EEPROM.read(i);
    n++;
  }
}

long ReadScore (int EepromAdr){
  return EEPROMReadLong(EepromAdr+10);
}

int CheckScore (long score){
  int p=1;
  int adr=20;
  long StoredScore;
  for(int i=20;i<181;i=i+20){
    StoredScore = EEPROMReadLong(i);
    if(StoredScore == 0)
      p=p;
    else if(score > StoredScore)
      p++;
  }
  return p;
}

int CheckName(){
  int p=1;
  for(int h=10;h<180;h = h + 20){
    ReadName(h, NameRead1);
    bool Equal = false;
    for(int k=0;k<8;k++){
      if(navn[k] != NameRead1[k]){
        Equal = false;
        p++;
        break;
      }
      else
        Equal = true;
    }
    if(Equal)
      return p;   
 }
 return 0; 
}

void DeleteScore(int p){
  byte d;
  int adr = p*20 - 10;
  for(int h=adr;h<=150;h=h+20){
    for(int i=0;i<20;i++){
    d = EEPROM.read(h + 20 + i);
    EEPROM.write(h + i, d);
    }
  }
  EEPROMWriteScore(170, "NoScore ", 0);
}

bool InsertScore(long Score){
  int CurrPos = CheckName();
  if(CurrPos != 0){
    if(Score < ReadScore(CurrPos*20-10))
      DeleteScore(CurrPos);
    else
      return false;
 }
  byte d;
  int p =  CheckScore(Score);
  int adr = p*20 - 10;
  if(p == 9)
    EEPROMWriteScore(adr, navn, Score);
  else if(p < 9){
    for(int h=170;h>=adr;h=h-20){
      for(int i=0;i<20;i++){
        d = EEPROM.read(h + i);
        EEPROM.write(h + 20 + i, d);
        }
    }
    EEPROMWriteScore(adr, navn, Score);
  }
  return true;
}

void displayTime(unsigned long millisek){
  unsigned long over;
  int num;
  
  num=int(millisek/600000);  // m_tens
  lc.setDigit(0,6,num,false);
  over=millisek%600000;
  num=int(over/60000);   // m_ones
  lc.setDigit(0,5,num,true);
  over=over%60000;
  num=int(over/10000);   // s_tens
  lc.setDigit(0,4,num,false);
  over=over%10000;
  num=int(over/1000);    // s_ones
  lc.setDigit(0,3,num,true);
  over=over%1000;
  num=int(over/100);  // ms_hundreds
  lc.setDigit(0,2,num,false);
  over=over%100;
  num=int(over/10);           // ms_tens
  lc.setDigit(0,1,num,false);
  over=over%10;
  num=int(over);              // ms_ones
  lc.setDigit(0,0,num,false);
  
} // displayTime

void timeToChar(unsigned long millisek, char *Time){

  unsigned long over;
  int num;
  char buff[10];

  num=int(millisek/600000);   //Minutes, tens (m_tens)
  itoa(num,buff,10);
  Time[0] = buff[0];          //Minutes, ones (m_ones)
  over=millisek%600000;
  num=int(over/60000);
  itoa(num,buff,10);
  Time[1] = buff[0];
  Time[2] = ':';
  over=over%60000;            //Seconds, tens (s_tens)
  num=int(over/10000);
  itoa(num,buff,10);
  Time[3] = buff[0];
  over=over%10000;            //Second, ones (s_ones)
  num=int(over/1000);
  itoa(num,buff,10);
  Time[4] = buff[0];
  Time[5] = ':';
  over=over%1000;             //Millisec, hundreds (ms_hundreds)
  num=int(over/100);
  itoa(num,buff,10);
  Time[6] = buff[0];
  over=over%100;              //Millisec, tens (ms_tens)
  num=int(over/10);
  itoa(num,buff,10);
  Time[7] = buff[0];
  over=over%10;               //Millisec, ones (ms_ones)
  num=int(over);
  itoa(num,buff,10);
  Time[8] = buff[0];

}

void successSound(){
  if(!mute){
    tone(Buzz, 523);
    delay(100);
    noTone(Buzz);
    delay(50);
    tone(Buzz, 1047);
    delay(500);
    noTone(Buzz);
    pinMode(Buzz, INPUT);
  }
}

void warningSound(){
  if(!mute){
    tone(Buzz, 196); // Note G3
    delay(100);
    noTone(Buzz);
    pinMode(Buzz, INPUT);
  }
}

void failedSound(){
  if(!mute){
    tone(Buzz, 98); // Note G2
    delay(500);
    noTone(Buzz);
    pinMode(Buzz, INPUT);
  }
}

void busted(){ // Interrupt function
  if(!nono){
    static unsigned long last_interrupt_time = 0;
    unsigned long interrupt_time = millis();
    if(interrupt_time - last_interrupt_time > 50){ // Debounce / delay 
      failed_no++;
      failed = true;
    }
    last_interrupt_time = interrupt_time;
  }
}

void loop() {

  newStart = false;
  showHighScoreNo = 1;
  ReadHighscores(10);
  updateOLED(1);
  

  while(digitalRead(StartPad1)==HIGH && digitalRead(StartPad2)==HIGH){ 
    flashHighScores++;
    if(flashHighScores == 30){
     showHighScoreNo = 4;
     ReadHighscores(70);
     updateOLED(1);
    }
    else if(flashHighScores == 60){
      showHighScoreNo = 7;
      ReadHighscores(130);
      updateOLED(1);
    }
    else if(flashHighScores == 90){
      showHighScoreNo = 1;
      ReadHighscores(10);
      updateOLED(1);
      flashHighScores=0;
    }
    else if(flashHighScores == 29 || flashHighScores == 59 || flashHighScores == 89){
      if(volt < 7.0){
        updateOLED(10); // LowBattery
        delay(1000);
      }
      if(batSaver > 1200){
        updateOLED(11);
        warningSound();
        delay(2000);
        batSaver = 1201;
      }
    }
      
    // Voltage
    if(ant_maalinger > 0){
      sum += analogRead(Volt);
      ant_maalinger--;
    }
    else{
      volt = (((sum / 5) * 5.10) / 1024.0) * 11.6;
      ant_maalinger = 5;
      sum = 0.0;
    }
      
    if(!digitalRead(SW)){
      updateOLED(9);
      delay(2000);
      if(!digitalRead(SW)){
        mute = !mute;
        updateOLED(9);
      }
      delay(2000);
    }
    
    delay(100);
    batSaver++;
  }
 
  delay(200);
  
  if(digitalRead(StartPad1)==LOW){
    lc.shutdown(0,false);
    updateOLED(2);
    while(digitalRead(StartPad1)==LOW){
      
    }
    nono = true;
    attachInterrupt(1, busted, CHANGE);
    updateOLED(3);
    Start = millis();
    nono = false;
    while(digitalRead(StartPad2) == HIGH && !newStart && (failed_no < 3)){
      Elapsed=millis() - Start; 
      displayTime(Elapsed);
      if(digitalRead(StartPad1) == LOW)
        newStart = true;
      if(failed){
        warningSound();
        failed = false;
        if(failed_no == 1)
          digitalWrite(Led1, HIGH);
        if(failed_no == 2)
          digitalWrite(Led2, HIGH);
      }
    }
  }
  
  else if(digitalRead(StartPad2)==LOW){
    lc.shutdown(0,false);
    updateOLED(2);
    while(digitalRead(StartPad2)==LOW){
      
    }
    nono = true;
    attachInterrupt(1, busted, CHANGE); // Interrupt
    updateOLED(3);
    Start = millis();
    nono = false;
    while(digitalRead(StartPad1) == HIGH && !newStart && (failed_no < 3)){
      Elapsed=millis() - Start; 
      displayTime(Elapsed);
      if(digitalRead(StartPad2) == LOW)
        newStart = true;
      if(failed){
        warningSound();
        failed = false;
        if(failed_no == 1)
          digitalWrite(Led1, HIGH);
        if(failed_no == 2)
          digitalWrite(Led2, HIGH);
      }
    }
  }

  detachInterrupt(1);
  
  if(failed_no > 2){
    failed_no = 0;
    digitalWrite(Led3, HIGH);
    updateOLED(5);
    failedSound();
    digitalWrite(Led1, LOW);
    digitalWrite(Led2, LOW);
    digitalWrite(Led3, LOW);
    delay(500);
    digitalWrite(Led1, HIGH);
    digitalWrite(Led2, HIGH);
    digitalWrite(Led3, HIGH);
    delay(500);
    digitalWrite(Led1, LOW);
    digitalWrite(Led2, LOW);
    digitalWrite(Led3, LOW);
    delay(500);
    digitalWrite(Led1, HIGH);
    digitalWrite(Led2, HIGH);
    digitalWrite(Led3, HIGH);
    delay(500);
    digitalWrite(Led1, LOW);
    digitalWrite(Led2, LOW);
    digitalWrite(Led3, LOW);
    lc.shutdown(0,true);
  }
  else if(newStart)
    newStart = false;
  else if(Elapsed > 0){
    listePlass = CheckScore(Elapsed);
    updateOLED(4);
    successSound();
    delay(4000);
    if(listePlass < 10){
      lc.shutdown(0,true);
      digitalWrite(Led1, LOW);
      digitalWrite(Led2, LOW);
      digitalWrite(Led3, LOW);
      if(listePlass == 1){
        writeName();
        InsertScore(Elapsed);
        for(int i=0;i<8;i++)
          navn[i] = ' ';
        nameSize = 0;
      }
      else if(saveHS()){
        writeName();
        InsertScore(Elapsed);
        for(int i=0;i<8;i++)
          navn[i] = ' ';
        nameSize = 0;
      }
    }
  }

  Elapsed=0; 
  displayTime(Elapsed);
  lc.shutdown(0,true);
  failed_no = 0;
  failed = false;
  digitalWrite(Led1, LOW);
  digitalWrite(Led2, LOW);
  digitalWrite(Led3, LOW);
  batSaver = 0;
}
