#include <HijelHID_BLEKeyboard.h>
#include <Wire.h>
#include "SSD1306Wire.h"

const int PLAY_PAUSE_PIN = 18;
const int VOL_DOWN_PIN = 5;
const int VOL_UP_PIN = 19;
const int numBtnPins = 3;

const int SCREEN_SDA_PIN = 23;
const int SCREEN_SCK_PIN = 22;

const int ALL_BTN_PINS[numBtnPins] = {
    PLAY_PAUSE_PIN,
    VOL_DOWN_PIN,
    VOL_UP_PIN
    };

HijelHID_BLEKeyboard keyboard;
SSD1306Wire display(0x3c, SCREEN_SDA_PIN, SCREEN_SCK_PIN, GEOMETRY_128_32);

void setPullupPins(const int pins[])
{
    for (int i = 0; i < numBtnPins; i++)
    {
        pinMode(pins[i], INPUT_PULLUP);
    }
}

void initializeDisplay()
{
    // Setup
    display.init();
    display.flipScreenVertically();
    display.setFont(ArialMT_Plain_16);
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);

    // Draw welcome text
    display.drawString(0, 0, "No song?");
    display.setTextAlignment(TEXT_ALIGN_CENTER);
    display.drawString(64, 15, ":(");
    // write the buffer to the display
    display.display();
}

void setup() {
    Serial.begin(115200);
    setPullupPins(ALL_BTN_PINS);

    keyboard.setLogLevel(HIDLogLevel::Normal);
    keyboard.begin();

    // TODO Test why this doesn't work \/\/\/\/
    // ----- DISPLAY TEST -----
    initializeDisplay();

    Serial.println("Ready. Pair via Bluetooth settings, then press BOOT to type.");
}

void loop() {
    // if (!keyboard.isPaired()) {
    //     static unsigned long lastPrint = 0;
    //     if (millis() - lastPrint > 3000) {
    //         Serial.println("Waiting for connection...");
    //         lastPrint = millis();
    //     }
    //     return;
    // }
    checkBtnPresses();
    delay(50);
}

// Checks all physical button presses and runs the corresponding action
void checkBtnPresses()
{
    pressBluetoothBtn(VOL_UP_PIN, MEDIA_VOLUME_UP);
    pressBluetoothBtn(VOL_DOWN_PIN, MEDIA_VOLUME_DOWN);
    pressBluetoothBtn(PLAY_PAUSE_PIN, MEDIA_PLAY_PAUSE);
}

// Check if physical btn pressed, if yes press corresponding bluetooth btn
void pressBluetoothBtn(const int pin, uint16_t bluetoothBtn)
{
    if (digitalRead(pin) == LOW) {
        keyboard.tap(bluetoothBtn, 25, 50);
        keyboard.releaseAll();
    }
}
