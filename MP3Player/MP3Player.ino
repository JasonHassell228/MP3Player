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
    String testText = "test text testy testy test text TEST TEST TESTY TESTY TEST TESTYYYYYYYYYYYYYYYYY";
    // TODO handle all UI display in separate helper func
    // ----- wip ui testing -----
    hf.display.clear(); // TODO clear with black rectangle to save energy & preserve header?
    hf.drawTextWithScroll(testText, 10, currentTime);
    hf.display.display();
    // ----- wip ui testing -----

    hf.checkBtnPresses(); // TODO: Run volume, play/pause etc on btn presses
    hf.webServer.handleClient();

    // TODO test code remove
    auto currentlyPlaying = hf.getSpotifyCurrentPlayingSong();
    Serial.println("GET SONG TEST: ");
    Serial.print("Song: ");
    Serial.println(currentlyPlaying.songName);
    Serial.print("Artist: ");
    Serial.println(currentlyPlaying.artist);
    delay(500);
    // TODO test code remove
    
    // TODO: getSpotifyCurrentPlayingSong needs to handle when no song playing
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
