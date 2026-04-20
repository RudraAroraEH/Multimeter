/*
  Ohmmeter with 470Ω reference on GP26 + OLED Display (128x32)
  Displays resistance, voltage, and ADC value on OLED.
  I2C pins: SDA=GP4, SCL=GP5
*/

#include <U8g2lib.h>
#include <Wire.h>

// OLED (I2C) – adjust pins if different
U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0);

const int ADC_PIN = 26;
const float R_REF = 470.0;   // your reference resistor (ohms)
const float VREF = 3.3;
const int ADC_MAX = 4095;

void setup() {
  Serial.begin(115200);
  analogReadResolution(12);
  pinMode(ADC_PIN, INPUT);

  // I2C for OLED
  Wire.setSDA(4);
  Wire.setSCL(5);
  Wire.begin();
  u8g2.begin();

  // Splash screen
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(0, 16, "Ohmmeter Ready");
  u8g2.sendBuffer();
  delay(1000);

  Serial.println("Ohmmeter Test");
}

void loop() {
  int adc = analogRead(ADC_PIN);
  float Vout = (adc / (float)ADC_MAX) * VREF;
  
  float Rx = 0;
  if (Vout >= VREF - 0.05) {
    Rx = INFINITY;
  } else if (Vout <= 0.01) {
    Rx = 0.0;
  } else {
    Rx = R_REF * (Vout / (VREF - Vout));
  }
  
  // ----- Serial output (unchanged) -----
  Serial.print("ADC: "); Serial.print(adc);
  Serial.print("  Vout: "); Serial.print(Vout, 3);
  Serial.print(" V  Rx: ");
  if (isinf(Rx)) Serial.println("OPEN");
  else if (Rx >= 1000.0) {
    Serial.print(Rx / 1000.0, 2);
    Serial.println(" kΩ");
  } else {
    Serial.print(Rx, 1);
    Serial.println(" Ω");
  }
  
  // ----- OLED display -----
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x10_tf);
  u8g2.drawStr(0, 10, "Resistance:");
  
  u8g2.setFont(u8g2_font_helvB12_tf);
  char buf[20];
  if (isinf(Rx)) {
    snprintf(buf, sizeof(buf), "OPEN");
  } else if (Rx >= 1000.0) {
    snprintf(buf, sizeof(buf), "%.2f kΩ", Rx / 1000.0);
  } else {
    snprintf(buf, sizeof(buf), "%.1f Ω", Rx);
  }
  u8g2.drawStr(0, 28, buf);
  
  
  u8g2.setFont(u8g2_font_6x10_tf);
  char adcBuf[10];
  snprintf(adcBuf, sizeof(adcBuf), "ADC:%d", adc);
  u8g2.drawStr(128 - u8g2.getStrWidth(adcBuf), 26, adcBuf);
  
  u8g2.sendBuffer();
  
  delay(500);
}