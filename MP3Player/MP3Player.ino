#include <HijelHID_BLEKeyboard.h>

HijelHID_BLEKeyboard keyboard;

const int PLAY_PAUSE_PIN = 18;
const int VOL_DOWN_PIN = 19;
const int VOL_UP_PIN = 5;
const int SCREEN_SDA_PIN = 23;
const int SCREEN_SCK_PIN = 22;

int ALL_PINS[] = { PLAY_PAUSE_PIN, VOL_DOWN_PIN, VOL_UP_PIN, SCREEN_SDA_PIN, SCREEN_SCK_PIN };

void SetPullupPins(int pins[])
{
    int numPins = sizeof(pins) / sizeof(pins[0]);
    for (int i = 0; i < numPins; i++)
    {
        pinMode(pins[i], INPUT_PULLUP);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("HijelHID BLE Keyboard — Basic Example");

    SetPullupPins(ALL_PINS);
    // pinMode(PLAY_PAUSE_PIN, INPUT_PULLUP);
    // pinMode(VOL_DOWN_PIN, INPUT_PULLUP);
    // pinMode(VOL_UP_PIN, INPUT_PULLUP);

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