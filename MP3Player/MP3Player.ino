#include <HijelHID_BLEKeyboard.h>

HijelHID_BLEKeyboard keyboard;

const int VOL_DOWN_PIN = 18;
const int VOL_UP_PIN = 19;

void setup() {
    Serial.begin(115200);
    Serial.println("HijelHID BLE Keyboard — Basic Example");

    pinMode(VOL_DOWN_PIN, INPUT_PULLUP);
    pinMode(VOL_UP_PIN, INPUT_PULLUP);

    keyboard.setLogLevel(HIDLogLevel::Normal);

    keyboard.begin();

    Serial.println("Ready. Pair via Bluetooth settings, then press BOOT to type.");
}

void loop() {
    if (!keyboard.isPaired()) {
        static unsigned long lastPrint = 0;
        if (millis() - lastPrint > 3000) {
            Serial.println("Waiting for connection...");
            lastPrint = millis();
        }
        return;
    }

    if (digitalRead(VOL_UP_PIN) == LOW) {
        keyboard.tap(MEDIA_VOLUME_UP);
        keyboard.releaseAll();
        delay(50); // debounce
    }

    if (digitalRead(VOL_DOWN_PIN) == LOW) {
        keyboard.tap(MEDIA_VOLUME_DOWN);
        keyboard.releaseAll();
        delay(50); // debounce
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