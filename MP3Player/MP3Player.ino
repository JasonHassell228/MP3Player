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
    hf.checkBtnPresses(); // TODO: Run volume, play/pause etc on btn presses
    hf.webServer.handleClient();

    // TODO: Use millis technique above to run getSpotifyCurrentPlayingSong every second or so
    // TODO: UI to display:
    // [ ] Current playing song, & artist (smaller, below song)
    // [ ] ^^ millis technique to display scrolling text for song name
    // [ ] ^^ millis technique to show little gif/animation when song is playing
    // [ ] ^^ millis technique to show current track progress bar
    // [ ] Wifi enabled/disabled symbol in header
    // [ ] Display local web server IP at startup on screen instead of serial print

    delay(50);
}
