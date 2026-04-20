#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Create display object (I2C address 0x3C is common for SSD1306)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

volatile unsigned long counter = 0;

const unsigned long SAMPLING_TIME = 2000; // milliseconds
const int pulsePin = 27; // GPIO15
const int oledSDA = 4;  // GPIO16
const int oledSCL = 5;  // GPIO17

void  pulseHandler() {
  counter++;
}

void setup() {
  Serial.begin(115200);

  // I2C setup for Pico
  Wire.setSDA(oledSDA);
  Wire.setSCL(oledSCL);
 Wire.begin();

  // OLED init
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.display();

  // Pulse input pin
  pinMode(pulsePin, INPUT_PULLDOWN);
  attachInterrupt(digitalPinToInterrupt(pulsePin), pulseHandler, RISING);

  Serial.println("waiting...");
}

void loop() {
  delay(SAMPLING_TIME);

  unsigned long pulse_per_sample;
  noInterrupts();
  pulse_per_sample = counter;
  counter = 0;
  interrupts();

  float pulse_per_sec = pulse_per_sample / (SAMPLING_TIME / 1000.0);
  Serial.print("Pulse Frequency (samples/sec): ");
  Serial.println(pulse_per_sec);

  display.clearDisplay();
  display.setCursor(0, 0);

  if (pulse_per_sec > 0) {
    float cap_uf = 1000.0 / (pulse_per_sec * 2.0);
    Serial.print("Capacitance (uF): ");
    Serial.println(cap_uf);

    display.println("Capacitance:");
    display.print(cap_uf, 2);
    display.println(" uF");
  } else {
    display.println("Capacitance:");
    display.println("---");
  }

  display.display();
}