#include <U8g2lib.h>
#include <Wire.h>
// OLED (128x32 I2C)
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);
const int btnUp = 14;
const int btnDown = 15;
const unsigned char ohm[] PROGMEM = {
  0x00,0x18,0x24,0x42,0x42,0x24,0x66,0x00
};
const unsigned char volt[] PROGMEM = {
  0x00,0x00,0x42,0x66,0x3c,0x18,0x00,0x00
};
const unsigned char Amp[] PROGMEM = {
  0xff,0x81,0x99,0x81,0xa5,0xa5,0x81,0xff
};
const unsigned char cap[] PROGMEM = {
  0x00,0x24,0x24,0xe7,0x24,0x24,0x00,0x00
};
const unsigned char continuity[] PROGMEM = {
  0x00,0x4c,0x64,0xf7,0x64,0x4c,0x00,0x00
};
const unsigned char scope[] PROGMEM = {
  0x00,0x00,0x00,0x22,0x55,0x88,0x00,0x00
};
const char *menuItems[] = {
  "Ammeter",
  "Ohmmeter",
  "Voltmeter",
  "Capacitance",
  "Continuity",
  "Scope"
};
const unsigned char* icons[] = {
  Amp,
  ohm,
  volt,
  cap,
  continuity,
  scope
};
const int menuLength = 6;
int selected = 0;

void setup() {
  pinMode(btnUp, INPUT_PULLUP);
  pinMode(btnDown, INPUT_PULLUP);
  u8g2.begin();
}
void loop() {
  handleInput();
  drawMenu();
}
void handleInput() {
  if (digitalRead(btnUp) == LOW) {
    selected--;
    if (selected < 0) selected = menuLength - 1;
    delay(180);
  }
  if (digitalRead(btnDown) == LOW) {
    selected++;
    if (selected >= menuLength) selected = 0;
    delay(180);
  }
}
void drawMenu() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  int start = selected - 1;
  if (start < 0) start = 0;
  if (start > menuLength - 3) start = menuLength - 3;
  for (int i = 0; i < 3; i++) {
    int index = start + i;
    int y = 10 + (i * 10);
    if (index == selected) {
      u8g2.drawStr(0, y, ">");
    }
    u8g2.drawXBMP(10, y - 8, 8, 8, icons[index]);
    u8g2.drawStr(22, y, menuItems[index]);
  }
  u8g2.sendBuffer();
}
