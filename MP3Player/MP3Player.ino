// Other libs imported in HelperFunctions.h
#include "HelperFunctions.h"

HelperFunctions hf;

void setup() {
  // General hardware setup
    Serial.begin(115200);
    delay(100); // Give serial time to init
    hf.setPullupPins();

    // Bluetooth keyboard setup
    hf.keyboard.setLogLevel(HIDLogLevel::Normal);
    hf.keyboard.begin();

    hf.initializeDisplay(); // 128x32 pixel display setup
    hf.setupWifi(); // Will block until complete
    Serial.println();

    hf.generateCodeChallenge(); // Generate and set verifier & challenge
    hf.setupWebServer();
}

void loop() {
    auto currentTime = millis();

    hf.checkBtnPresses(); // TODO: Run volume, play/pause etc on btn presses
    hf.webServer.handleClient();
    hf.spotifyLoop(currentTime);

    // TODO: UI to display:
    // [ ] Current playing song, & artist (smaller, below song) 
    // [x] ^^ millis technique to display scrolling text for song name
    // [ ] ^^ millis technique to show little gif/animation when song is playing
    // [ ] ^^ millis technique to show current track progress bar
    // [ ] Wifi enabled/disabled symbol in header
    // [ ] Display local web server IP at startup on screen instead of serial print

    delay(50);
}
