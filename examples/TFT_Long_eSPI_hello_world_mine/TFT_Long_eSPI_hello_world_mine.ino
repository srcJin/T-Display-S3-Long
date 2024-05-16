/*
  For Lilygo T-Display-S3-Long
  nikthefix - 28th Nov 2023
  Modified driver code and example sketch using TFT_eSPI in 'sprite only mode'

  Versions:
  TFT_eSPI 2.5.34 - latest at time of writing
  ESP32 Arduino 3.0.0-alpha2 - latest at time of writing

  Notes:
  As the display doesn't implement a scan orientation hardware rotate - as far as I can see from the current datasheet - we need to use
  a soft matrix rotation to get a landscape view without messing with TFT_eSPI. This is implemented in lcd_PushColors_rotated_90().
  You can then use lcd_setRotation(2) which IS hardware implemented, to flip the whole thing upside down if you need.
  Code has been stripped down to support QSPI display only.

  Build Options:
  Board  ESP32-S3-Dev
  USB CDC On boot Enabled
  Flash Size 16MB
  Partition Scheme 16M Flash(3MB APP/9.9MB FATFS)
  PSRAM "OPI PSRAM"

  Since ESP32 Arduino 3.0.0-alpha3 is still pretty funky with a lot of existing Arduino driver code it may be necessary to downgrade to V2.xx as the project expands - in the short term
*/

#include "AXS15231B.h"
#include <Arduino.h>
#include <TFT_eSPI.h>
// For bitmap encoding: use Image2lcd, 16bit true colour, MSB First, RGB565, don't include head data, be sure to set max image size, save as .h file.

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite sprite = TFT_eSprite(&tft);

int incomingByte; // a variable to read incoming serial data into
const int ledPin = 4;

String incomingString = "";
boolean stringComplete = false;

void draw()
{
    sprite.fillSprite(TFT_BLACK);
    sprite.drawString("Interaction Intelligence", 220, 20, 4);
    sprite.fillRect(210, 100, 60, 60, TFT_RED);
    sprite.fillRect(280, 100, 60, 60, TFT_GREEN);
    sprite.fillRect(350, 100, 60, 60, TFT_BLUE);

    lcd_PushColors_rotated_90(0, 0, 640, 180, (uint16_t *)sprite.getPointer());
}

void setup()
{
    pinMode(TFT_BL, OUTPUT);
    digitalWrite(TFT_BL, LOW); // turn off backlight asap to minimise power on artifacts
    Serial.begin(9600);
    Serial.setDebugOutput(true);

    Serial.print("In Setup");
    axs15231_init();
    // lcd_setRotation(2);             // 180 degree hardware rotate if you want reset / boot buttons at the bottom
    sprite.createSprite(640, 180); // full screen landscape sprite in psram
    sprite.setSwapBytes(1);
    lcd_fill(0, 0, 180, 640, 0x00); // clear screen
    digitalWrite(TFT_BL, HIGH);     // turn on backlight
    Serial.print("END SETUP");
}

void loop()
{

    draw();
    delay(300);

    Serial.print("In Loop");

    // sprite.fillSprite(TFT_CYAN);
    // lcd_PushColors_rotated_90(0, 0, 640, 180, (uint16_t *)sprite.getPointer());
    // delay(1000);

    // see if there's incoming serial data:
    while (Serial.available())
    {
        // Get the next character
        char inChar = (char)Serial.read();
        // Add it to the string
        incomingString += inChar;
        // If the incoming character is a newline, set stringComplete flag
        Serial.print("curret_incomingString=");
        Serial.print(incomingString);

        if (inChar == '.')
        {
            stringComplete = true;
            Serial.print("stringComplete");

        }
    }

    // Process the string if it's complete
  if (stringComplete) {
    // Print the complete string
    Serial.print("Received: ");
    Serial.println(incomingString);
    sprite.fillSprite(TFT_BLACK); // Clear the entire sprite
    sprite.drawString(incomingString, 10, 10, 4); // Adjust position and font size
    lcd_PushColors_rotated_90(0, 0, 640, 180, (uint16_t *)sprite.getPointer());
    delay(1000);
    sprite.fillSprite(TFT_CYAN);
    lcd_PushColors_rotated_90(0, 0, 640, 180, (uint16_t *)sprite.getPointer());
    delay(1000);

    // Check for specific commands in the string
    if (incomingString.indexOf('H') >= 0) {
      digitalWrite(LED_BUILTIN, HIGH);
    }
    if (incomingString.indexOf('L') >= 0) {
      digitalWrite(LED_BUILTIN, LOW);
    }

    // Clear the string and reset the flag
    incomingString = "";
    stringComplete = false;
  }
}
