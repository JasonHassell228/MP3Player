// Other libs imported in HelperFunctions.h
#include "HelperFunctions.h"

HelperFunctions hf;

SSD1306Wire display(0x3c, hf.SCREEN_SDA_PIN, hf.SCREEN_SCK_PIN, GEOMETRY_128_32);

unsigned long lastScrollUIText = 0;
int scrollIndexUIX = 128;
int screenWidth = 128;

void initializeDisplay()
{
    // Setup
    display.init();
    display.flipScreenVertically();
    display.setFont(Monospaced_plain_8);
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);

    // Draw welcome text
    display.drawString(0, 10, "No song? No song? No song?");
    display.drawString(0, 20, ":(");
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
    auto currentTime = millis();
    String testText = "test text testy testy test text TEST TEST TESTY TESTY TEST TESTYYYYYYYYYYYYYYYYY";
    auto totalWidth = display.getStringWidth(testText);
    if (totalWidth > screenWidth) {
        if ((currentTime - lastScrollUIText) >= 30) {
            lastScrollUIText = currentTime;
            display.clear(); // TODO clear with black rectangle to save energy & preserve header?
            display.drawString(scrollIndexUIX, 10, testText);
            display.display();
            scrollIndexUIX--;
            if (scrollIndexUIX < (-totalWidth + screenWidth - 20)) {
                delay(3000); // TODO BLOCKING - set new millis time instead
                scrollIndexUIX = screenWidth;
            }
        }
    }
    // ----- wip UI scroll testing

    hf.checkBtnPresses(); // TODO: Run volume, play/pause etc on btn presses
    hf.webServer.handleClient();

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
