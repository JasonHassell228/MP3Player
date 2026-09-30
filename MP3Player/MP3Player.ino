#include <HijelHID_BLEKeyboard.h>
#include <WiFiClientSecure.h>
#include <WiFi.h>
#include <Wire.h>
#include "SSD1306Wire.h"

// ----- wip wifi test -----
const char* ssid     = "xx";     // your network SSID (name of wifi network)
const char* password = "xx"; // your network password
const char*  server = "www.howsmyssl.com";  // Server URL
WiFiClientSecure client;
// ----- wip wifi test -----

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
    delay(100); // Give serial time to init
    setPullupPins(ALL_BTN_PINS);

    keyboard.setLogLevel(HIDLogLevel::Normal);
    keyboard.begin();

    // TODO Test why this doesn't work \/\/\/\/
    // ----- DISPLAY TEST -----
    initializeDisplay();

    Serial.println("Ready. Pair via Bluetooth settings, then press BOOT to type.");
    // ----- wip wifi test -----
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);

    // attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        // wait 1 second for re-trying
        delay(1000);
    }

    Serial.print("Connected to ");
    Serial.println(ssid);

    Serial.println("\nStarting connection to server...");
    client.setInsecure();//skip verification
    if (!client.connect(server, 443))
        Serial.println("Connection failed!");
    else {
        Serial.println("Connected to server!");
        // Make a HTTP request:
        client.println("GET https://www.howsmyssl.com/a/check HTTP/1.0");
        client.println("Host: www.howsmyssl.com");
        client.println("Connection: close");
        client.println();

        while (client.connected()) {
        String line = client.readStringUntil('\n');
        if (line == "\r") {
            Serial.println("headers received");
            break;
        }
        }
        // if there are incoming bytes available
        // from the server, read them and print them:
        while (client.available()) {
        char c = client.read();
        Serial.write(c);
        }

        client.stop();
    }
    // ----- wip wifi test -----
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
