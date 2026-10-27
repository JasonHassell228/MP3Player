// Other libs imported in HelperFunctions.h
#include <WiFi.h>
#include <Wire.h>
#include "HelperFunctions.h"
#include "SSD1306Wire.h"

HelperFunctions hf;
const int ALL_BTN_PINS[hf.numBtnPins] = {
    hf.PLAY_PAUSE_PIN,
    hf.VOL_DOWN_PIN,
    hf.VOL_UP_PIN
};

SSD1306Wire display(0x3c, hf.SCREEN_SDA_PIN, hf.SCREEN_SCK_PIN, GEOMETRY_128_32);

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
    hf.setPullupPins(ALL_BTN_PINS);

    hf.keyboard.setLogLevel(HIDLogLevel::Normal);
    hf.keyboard.begin();

    // TODO Test why this doesn't work \/\/\/\/
    // ----- DISPLAY TEST -----
    initializeDisplay();

    Serial.println("Ready. Pair via Bluetooth settings, then press BOOT to type.");
    // ----- wip wifi test -----
    Serial.print("Attempting to connect to SSID: ");
    Serial.println(hf.ssid);
    WiFi.begin(hf.ssid, hf.password);

    // attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        // wait 1 second for re-trying
        delay(1000);
    }

    Serial.print("Connected to ");
    Serial.println(hf.ssid);

    Serial.println("\nStarting connection to server...");
    hf.client.setInsecure();//skip verification
    if (!hf.client.connect(hf.server, 443))
        Serial.println("Connection failed!");
    else {
        Serial.println("Connected to server!");
        // Make a HTTP request:
        hf.client.println("GET https://www.howsmyssl.com/a/check HTTP/1.0");
        hf.client.println("Host: www.howsmyssl.com");
        hf.client.println("Connection: close");
        hf.client.println();

        while (hf.client.connected()) {
        String line = hf.client.readStringUntil('\n');
        if (line == "\r") {
            Serial.println("headers received");
            break;
        }
        }
        // if there are incoming bytes available
        // from the server, read them and print them:
        while (hf.client.available()) {
        char c = hf.client.read();
        Serial.write(c);
        }

        hf.client.stop();
    }
    // ----- wip wifi test -----

    // ----- wip spotify test -----
    /* todo (https://developer.spotify.com/documentation/web-api/tutorials/code-pkce-flow): 
    [x] generate code verifier
    [ ] generate code challenge (SHA 256 hash) (+ base64 encode?)
    [ ] GET request to /authorize endpoint
    [ ] exchange received auth code for access token
    */
    Serial.println();
    Serial.println("access token: " + hf.getAccessToken());
    String codeVerifier = hf.generateCodeVerifier(64);

    // ----- wip base64 encode test -----
    std::vector<uint8_t> data(codeVerifier.begin(), codeVerifier.end());
	auto encoded = hf.encode_base64(data);
	auto decoded = hf.decode_base64(encoded);

	Serial.printf("Original (%d bytes): ", data.size());
	hf.printBlockAsUtf8(data);
	Serial.println();

	Serial.printf("Encoded (%d bytes): ", encoded.length());
	Serial.print(encoded.c_str());
	Serial.println();

	Serial.printf("Decoded (%d bytes): ", decoded.size());
	hf.printBlockAsUtf8(decoded);
    // ----- wip base64 encode test -----

    std::string cStrCodeVerifier = codeVerifier.c_str();
    auto codeVerifiedHashed = hf.generateSHA256(cStrCodeVerifier);
    Serial.println("code verifier: " + cStrCodeVerifier);
    Serial.println("code verifier SHA256: ");
    for (uint8_t byte : codeVerifiedHashed) {
        Serial.print(byte);
        Serial.print(" ");
    }
    Serial.println();
    // ----- wip spotify test -----
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
    hf.checkBtnPresses();
    delay(50);
}
