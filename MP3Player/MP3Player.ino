// Other libs imported in HelperFunctions.h
#include "HelperFunctions.h"

HelperFunctions hf;

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
  // General hardware setup
    Serial.begin(115200);
    delay(100); // Give serial time to init
    hf.setPullupPins();

    // Bluetooth keyboard setup
    hf.keyboard.setLogLevel(HIDLogLevel::Normal);
    hf.keyboard.begin();

    initializeDisplay(); // 128x32 pixel display setup
    hf.setupWifi(); // Will block until complete
    Serial.println();

    hf.generateCodeChallenge(); // Generate and set verifier & challenge
    hf.setupWebServer();
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
    hf.webServer.handleClient();

    delay(50);
}
