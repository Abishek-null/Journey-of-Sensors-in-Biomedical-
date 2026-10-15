#include <Wire.h>
#include "MAX30100_PulseOximeter.h"
#include <Adafruit_GFX.h>
#include <Adafruit_SH110X.h>

// =================================================
// CONFIGURATION
// =================================================

#define SDA_PIN             21
#define SCL_PIN             22

#define OLED_ADDRESS        0x3C            //OLED Addresss
#define SCREEN_WIDTH        128            //Screen Width
#define SCREEN_HEIGHT       64

#define DISPLAY_INTERVAL    500
#define SERIAL_INTERVAL     1000
#define BEAT_TIMEOUT        3000

// =================================================
// OBJECTS
// =================================================

Adafruit_SH1106G display(
    SCREEN_WIDTH,
    SCREEN_HEIGHT,
    &Wire,
    -1
);

PulseOximeter pox;

// =================================================
// VARIABLES
// =================================================

uint32_t lastDisplayUpdate = 0;
uint32_t lastSerialUpdate = 0;

uint32_t lastBeatTime = 0;
uint32_t beatFlashUntil = 0;

float heartRate = 0.0f;
float spo2 = 0.0f;

bool sensorReady = false;
bool oledReady = false;

// =================================================
// HEARTBEAT CALLBACK
// =================================================

void onBeatDetected()
{
    lastBeatTime = millis();
    beatFlashUntil = millis() + 180;

    Serial.println("Heartbeat detected!");
}

// =================================================
// HEART ICON
// =================================================

void drawHeart(int x, int y, bool filled)
{
    if (filled)
    {
        display.fillCircle(x + 4, y + 4, 4, SH110X_WHITE);
        display.fillCircle(x + 12, y + 4, 4, SH110X_WHITE);

        display.fillTriangle(
            x, y + 5,
            x + 16, y + 5,
            x + 8, y + 14,
            SH110X_WHITE
        );
    }
    else
    {
        display.drawCircle(x + 4, y + 4, 4, SH110X_WHITE);
        display.drawCircle(x + 12, y + 4, 4, SH110X_WHITE);

        display.drawLine(
            x, y + 5,
            x + 8, y + 14,
            SH110X_WHITE
        );

        display.drawLine(
            x + 8, y + 14,
            x + 16, y + 5,
            SH110X_WHITE
        );
    }
}

// =================================================
// OLED DISPLAY
// =================================================

void drawMonitor()
{
    if (!oledReady)
        return;

    // Use the same validity conditions as Serial Monitor
    bool validHR =
        (heartRate >= 30.0f && heartRate <= 220.0f);

    bool validSpO2 =
        (spo2 >= 70.0f && spo2 <= 100.0f);

    display.clearDisplay();
    display.setTextColor(SH110X_WHITE);

    // HEADER
    display.setTextSize(1);
    display.setCursor(20, 2);
    display.print("HEALTH MONITOR");

    display.drawLine(0, 12, 127, 12, SH110X_WHITE);
    display.drawLine(64, 16, 64, 47, SH110X_WHITE);

    // HEART RATE
    display.setTextSize(1);
    display.setCursor(4, 17);
    display.print("HEART RATE");

    display.setTextSize(2);
    display.setCursor(4, 29);

    if (validHR)
    {
        display.print((int)(heartRate + 0.5f));
    }
    else
    {
        display.print("--");
    }

    display.setTextSize(1);
    display.setCursor(5, 46);
    display.print("BPM");

    // SPO2
    display.setTextSize(1);
    display.setCursor(78, 17);
    display.print("SpO2");

    display.setTextSize(2);
    display.setCursor(70, 29);

    if (validSpO2)
    {
        display.print((int)(spo2 + 0.5f));
        display.print("%");
    }
    else
    {
        display.print("--%");
    }

    // STATUS BAR
    display.drawLine(0, 51, 127, 51, SH110X_WHITE);

    display.setTextSize(1);
    display.setCursor(3, 55);

    if (!validHR && !validSpO2)
    {
        display.print("MEASURING...");
    }
    else if (!validHR)
    {
        display.print("CHECKING PULSE");
    }
    else if (!validSpO2)
    {
        display.print("CHECKING SPO2");
    }
    else
    {
        display.print("LIVE READING");
    }

    // HEARTBEAT ANIMATION
    drawHeart(
        106,
        53,
        millis() < beatFlashUntil
    );

    // SEND BUFFER TO OLED
    display.display();
}
// =================================================
// SETUP
// =================================================

void setup()
{
    Serial.begin(115200);
    delay(300);

    Serial.println();
    Serial.println("ESP32 HEART RATE + SpO2 MONITOR");

    // Initialize I2C
    Wire.begin(SDA_PIN, SCL_PIN);
    Wire.setClock(100000);

    // Initialize OLED
    oledReady = display.begin(OLED_ADDRESS, true);

    if (oledReady)
    {
        display.clearDisplay();
        display.setTextColor(SH110X_WHITE);
        display.setTextSize(1);
        display.setCursor(20, 20);
        display.println("HEALTH MONITOR");
        display.setCursor(25, 36);
        display.println("Initializing...");
        display.display();

        Serial.println("OLED ready.");
    }
    else
    {
        Serial.println("OLED initialization failed.");
    }

    // Initialize MAX30100
    if (!pox.begin())
    {
        Serial.println("MAX30100 initialization failed!");

        if (oledReady)
        {
            display.clearDisplay();
            display.setTextSize(1);
            display.setCursor(10, 22);
            display.println("SENSOR ERROR");
            display.setCursor(5, 38);
            display.println("Check MAX30100");
            display.display();
        }

        return;
    }

    // Moderate increase in IR LED current
    pox.setIRLedCurrent(MAX30100_LED_CURR_11MA);

    // Register heartbeat callback
    pox.setOnBeatDetectedCallback(onBeatDetected);

    sensorReady = true;

    Serial.println("MAX30100 ready.");
    Serial.println("Place finger gently and keep still.");

    // Do not initialize lastBeatTime here.
    // A beat must actually be detected first.
    lastBeatTime = 0;

    drawMonitor();
}

// =================================================
// MAIN LOOP
// =================================================

void loop()
{
    if (!sensorReady)
    {
        delay(100);
        return;
    }

    // IMPORTANT: keep this running as frequently as possible.
    pox.update();

    uint32_t now = millis();

    // Read sensor values and refresh display
    if (now - lastDisplayUpdate >= DISPLAY_INTERVAL)
    {
        lastDisplayUpdate = now;

        heartRate = pox.getHeartRate();
        spo2 = pox.getSpO2();

        drawMonitor();
    }

    // Serial output
    if (now - lastSerialUpdate >= SERIAL_INTERVAL)
    {
        lastSerialUpdate = now;

        Serial.print("Heart Rate: ");

        if (heartRate >= 30.0f && heartRate <= 220.0f)
        {
            Serial.print(heartRate, 1);
            Serial.print(" BPM");
        }
        else
        {
            Serial.print("-- BPM");
        }

        Serial.print(" | SpO2: ");

        if (spo2 >= 70.0f && spo2 <= 100.0f)
        {
            Serial.print(spo2, 1);
            Serial.println("%");
        }
        else
        {
            Serial.println("--%");
        }
    }
}