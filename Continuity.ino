#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---- Display ----
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  32
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---- Pins ----
#define ADC_PIN        26     
#define PIN_555_RESET  15      

// ---- Settings ----
#define R_KNOWN        1000.0  // 1k resistor to 3.3V
#define VREF           3.3
#define CONT_THRESHOLD 50.0    // Below 50 Ohm = continuity

float calcResistance()
{
    int raw = analogRead(ADC_PIN);
    if (raw <= 0) return 99999;
    float Vout = raw * VREF / 1023.0;
    return R_KNOWN * Vout / (VREF - Vout);
}

void setup()
{
    Serial.begin(9600);
    Wire.begin();

    pinMode(PIN_555_RESET, OUTPUT);
    digitalWrite(PIN_555_RESET, LOW);   // Silent on start

    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("OLED not found!");
        while (true);
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(20, 5);
    display.println("Continuity Tester");
    display.setCursor(30, 18);
    display.println("Ready...");
    display.display();
    delay(1500);
}

void loop()
{
    float R = calcResistance();
    bool connected = (R <= CONT_THRESHOLD);

    // Control 555
    digitalWrite(PIN_555_RESET, connected ? HIGH : LOW);

    // OLED
    display.clearDisplay();
    display.setTextSize(1);
    display.setCursor(0, 0);
    display.print("Continuity Test");

    display.setCursor(0, 20);
    if (connected) {
        display.print(">>> BEEP! CONNECTED");
    } else {
        display.print("    OPEN CIRCUIT");
    }
    display.display();

    // Serial
    Serial.print("R: ");
    if (R >= 99999) Serial.print("OPEN");
    else { Serial.print(R, 1); Serial.print(" Ohm"); }
    Serial.print(" | ");
    Serial.println(connected ? "CONNECTED - BEEPING" : "OPEN");

    delay(100);
}