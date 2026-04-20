#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---- Display Setup ----
#define SCREEN_WIDTH   128
#define SCREEN_HEIGHT  32
#define OLED_RESET     -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---- Scope Setup ----
#define ADC_PIN      26
#define NUM_SAMPLES  128        // One sample per pixel column
#define VREF         3.3

int   samples[NUM_SAMPLES];
float timeDiv    = 100;         // Microseconds between samples
bool  autoTrigger = true;

// ---- Capture samples ----
void captureSamples()
{
    if (autoTrigger) {
        unsigned long timeout = millis() + 200;     // 200ms timeout
        while (millis() < timeout) {
            int a = analogRead(ADC_PIN);
            int b = analogRead(ADC_PIN);
            if (a < 512 && b >= 512) break;         // Rising edge detected
        }
    }

    // Sample
    for (int i = 0; i < NUM_SAMPLES; i++) {
        samples[i] = analogRead(ADC_PIN);
        delayMicroseconds((int)timeDiv);
    }
}

// ---- Draw waveform on OLED ----
void drawScope()
{
    display.clearDisplay();

    // Draw center line (ground reference)
    for (int x = 0; x < SCREEN_WIDTH; x += 4) {
        display.drawPixel(x, SCREEN_HEIGHT / 2, SSD1306_WHITE);
    }

    // Draw waveform
    for (int i = 0; i < NUM_SAMPLES - 1; i++) {
        int y1 = map(samples[i],     0, 1023, SCREEN_HEIGHT - 1, 0);
        int y2 = map(samples[i + 1], 0, 1023, SCREEN_HEIGHT - 1, 0);
        display.drawLine(i, y1, i + 1, y2, SSD1306_WHITE);
    }

    // Draw stats top right
    float vMax = 0, vMin = 3.3, vAvg = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        float v = samples[i] * VREF / 1023.0;
        if (v > vMax) vMax = v;
        if (v < vMin) vMin = v;
        vAvg += v;
    }
    vAvg /= NUM_SAMPLES;

    // Overlay text
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Mx:");
    display.print(vMax, 1);
    display.print("V");

    display.setCursor(0, 24);
    display.print("Mn:");
    display.print(vMin, 1);
    display.print("V");

    display.setCursor(80, 0);
    display.print("Av:");
    display.print(vAvg, 1);
    display.print("V");

    // Time division label
    display.setCursor(80, 24);
    if (timeDiv >= 1000) {
        display.print((int)(timeDiv / 1000));
        display.print("ms/d");
    } else {
        display.print((int)timeDiv);
        display.print("us/d");
    }

    display.display();
}

// ---- Print to Serial ----
void printSerial(float vMax, float vMin, float vAvg)
{
    Serial.println("==============================");
    Serial.print  ("  Vmax : "); Serial.print(vMax, 3); Serial.println(" V");
    Serial.print  ("  Vmin : "); Serial.print(vMin, 3); Serial.println(" V");
    Serial.print  ("  Vavg : "); Serial.print(vAvg, 3); Serial.println(" V");
    Serial.print  ("  Vpp  : "); Serial.print(vMax - vMin, 3); Serial.println(" V");
    Serial.print  ("  T/div: "); Serial.print((int)timeDiv);   Serial.println(" us");
    Serial.println("==============================");
    Serial.println();
}

void setup()
{
    Serial.begin(500000);
    Wire.begin();
    pinMode(ADC_PIN, INPUT);

    if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
        Serial.println("OLED not found!");
        while (true);
    }

    display.clearDisplay();
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(20, 0);
    display.println("** Pico Scope **");
    display.setCursor(15, 11);
    display.println("Serial: 500000 baud");
    display.setCursor(30, 22);
    display.println("Starting...");
    display.display();

    Serial.println("  Send + to increase timeDiv  ");
    Serial.println("  Send - to decrease timeDiv  ");
    Serial.println("  Send t to toggle trigger    ");

    delay(2000);
}

void loop()
{
    // Serial commands to adjust timebase
    if (Serial.available()) {
        char cmd = Serial.read();
        if (cmd == '+') {
            timeDiv *= 2;
            if (timeDiv > 50000) timeDiv = 50000;
            Serial.print("Time/div: "); Serial.print((int)timeDiv); Serial.println("us");
        }
        if (cmd == '-') {
            timeDiv /= 2;
            if (timeDiv < 10) timeDiv = 10;
            Serial.print("Time/div: "); Serial.print((int)timeDiv); Serial.println("us");
        }
        if (cmd == 't') {
            autoTrigger = !autoTrigger;
            Serial.print("Trigger: ");
            Serial.println(autoTrigger ? "ON" : "OFF");
        }
    }

    captureSamples();
    drawScope();

    // Calc stats for serial
    float vMax = 0, vMin = 3.3, vAvg = 0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        float v = samples[i] * VREF / 1023.0;
        if (v > vMax) vMax = v;
        if (v < vMin) vMin = v;
        vAvg += v;
    }
    vAvg /= NUM_SAMPLES;
    printSerial(vMax, vMin, vAvg);
}