# Buzz Wire
This repo holds the code, wiring diagram and PCB files to build a Arduino Buzz wire game with timer and highscore as shown in this video: https://youtu.be/0P0GF0H_dCA?si=xbg_11h1RosW1U28

Follwing parts are required:  
1. Some brake pipe or copper pipe
2. Some copper wire for the start/stop pads and the ring/handle
3. Adafruit Metro Mini 328 (or similar Arduino board)
4. OLED with I2C bus, 0,96" 128x64 SSD1306
5. 8 digit segmet led display, with the MAX7219 driver
6. Rotary Encoder with button, KY-040
7. Regular 5mm red LEDs
8. Resistors as defined in the wiring diagram
9. DC-DC to 5v, f.ex Traco Power TSR 1-2450
10. Battery (I used 2x 3xAA battery holders as seen in the video)
11. Power switch
12. PCB, as defined by gerber files. You could also use a prototyping board for this, as circuit is very simple

I recommend to keep the copper "track" a bit easy, as this makes it more fun to compete on time. 
I think the track used in the video was a bit too hard.

**Note:**
The code relies on following library's:  
u8g2 (https://www.arduino.cc/reference/en/libraries/u8g2/)  Big thanks to Oli Kraus for developing and sharing this library!  
LedControl (https://www.arduino.cc/reference/en/libraries/ledcontrol/)  
EEPROM (https://docs.arduino.cc/learn/built-in-libraries/eeprom/)  

Remember to upload the ResetEEPROM code first to initialize the high score list, secondly you upload the main Buzz Wire code.  

Game on!
