#include <Arduino.h>
// This example is based on this tutorial
// https://www.instructables.com/id/Converting-Images-to-Flash-Memory-Iconsimages-for-/
// https://github.com/STEMpedia/eviveProjects/blob/master/imageToFlashMemoryIconsForTFT/

//Don't forget to change User_Setup.h inside TFT_eSPI library !

#include <TFT_eSPI.h>
// #include <Wire.h>
#include "bitmap.h" //(Bitmap generated with LCD Image Converter) https://sourceforge.net/projects/lcd-image-converter/
#include <Button2.h>

#define BUTTON_PIN  0
Button2 button = Button2(BUTTON_PIN);

TFT_eSPI tft = TFT_eSPI(); // Invoke library

void handler(Button2& btn) {
    switch (btn.getClickType()) {
        case SINGLE_CLICK:
        Serial.print("single ");
        tft.pushImage(0, 0, 240, 240, xiayu);
            break;
        case DOUBLE_CLICK:
            Serial.print("double ");
            tft.pushImage(0, 0, 240, 240, duoyun);
            break;
        case TRIPLE_CLICK:
            Serial.print("triple ");
         
            break;
        case LONG_CLICK:
            Serial.print("long");
            tft.pushImage(0, 0, 240, 240, yewan);
            break;
    }
    Serial.print("click");
    Serial.print(" (");
    Serial.print(btn.getNumberOfClicks());    
    Serial.println(")");
}

void setup(void)
{
  Serial.begin(115200);
  Serial.print("ST7789 TFT Bitmap Test");

button.setClickHandler(handler);
  button.setLongClickHandler(handler);
  button.setDoubleClickHandler(handler);
  button.setTripleClickHandler(handler);

  tft.begin();            // initialize a ST7789 chip
  tft.setSwapBytes(true); // Swap the byte order for pushImage() - corrects endianness

  tft.fillScreen(TFT_BLACK);

  //反转显示颜色i = 1反转，i = 0正常
  tft.invertDisplay(1);

  tft.setSwapBytes(true);
}

void loop()
{
button.loop();
  // buttonLeft.loop();
  // buttonRight.loop();
  // //反转显示颜色i = 1反转，i = 0正常
  // tft.invertDisplay(1);

  // tft.pushImage(0, 0, 240, 240, xiayu);
  // delay(1000);
  // tft.pushImage(0, 0, 240, 240, duoyun);
  // delay(1000);

  // tft.fillScreen(TFT_WHITE);
  // tft.drawCircle(120, 120, 55, TFT_BLACK);
  // delay(5000);
  // tft.pushImage(0, 0, 100, 100, feng);
  // delay(2000);
}
// void click(Button2& btn) {
//     if (btn == buttonLeft) {
//       Serial.println("A clicked");
//       tft.pushImage(0, 0, 240, 240, duoyun);
//     } else if (btn == buttonRight) {
//       Serial.println("B clicked");
//       tft.pushImage(0, 0, 240, 240, xiayu);
//     }
// }