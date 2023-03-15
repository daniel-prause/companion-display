#include <SPI.h>

#include <TFT_eSPI.h> // Hardware-specific library

TFT_eSPI tft = TFT_eSPI(); // Invoke custom library

void setup(void)
{
  tft.setRotation(5);
  tft.init();

  tft.fillScreen(TFT_BLACK);
  tft.drawRect(0, 0, 320, 170, TFT_NAVY);

  // Set "cursor" at top left corner of display (0,0) and select font 4
  tft.setCursor(0, 0, 4);

  // Set the font colour to be white with a black background
  tft.setTextColor(TFT_WHITE);
  tft.println("TEST Hallo!");
  delay(5000);
}

void loop()
{
  delay(5000);
}