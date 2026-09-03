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

void SetPullupPins(const int pins[])
{
    for (int i = 0; i < numBtnPins; i++)
    {
        pinMode(pins[i], INPUT_PULLUP);
    }
}

void setup() {
    Serial.begin(115200);
    SetPullupPins(ALL_BTN_PINS);

    keyboard.setLogLevel(HIDLogLevel::Normal);
    keyboard.begin();

    // TODO Test why this doesn't work \/\/\/\/
    // ----- DISPLAY TEST -----
        display.init();
        display.flipScreenVertically();
        display.setFont(ArialMT_Plain_24);
        // loop
        display.clear();
        display.setTextAlignment(TEXT_ALIGN_LEFT);
        display.drawString(0, 0, "Left aligned (0,0) blah blah");
        // write the buffer to the display
        display.display();
        // todo loop

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
    SetPullupPins(ALL_BTN_PINS);
    if (digitalRead(VOL_UP_PIN) == LOW) {
        keyboard.tap(MEDIA_VOLUME_UP, 25, 50);
        keyboard.releaseAll();
        delay(50); // debounce
    }

    if (digitalRead(VOL_DOWN_PIN) == LOW) {
        keyboard.tap(MEDIA_VOLUME_DOWN, 25, 50);
        keyboard.releaseAll();
        delay(50); // debounce
    }

    if (digitalRead(PLAY_PAUSE_PIN) == LOW) {
        keyboard.tap(MEDIA_PLAY_PAUSE, 25, 50);
        keyboard.releaseAll();
    }
    // printPin(VOL_DOWN_PIN);
    // printPin(VOL_UP_PIN);
    delay(50);
}

// void printPin(int pin)
// {
//     Serial.print("pin ");
//     Serial.print(pin);
//     Serial.print(": ");
//     Serial.println(digitalRead(pin));
// }