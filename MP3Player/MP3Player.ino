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

    // Wifi setup
    hf.setupWifi(); // Will block until complete

    /* todo (https://developer.spotify.com/documentation/web-api/tutorials/code-pkce-flow): 
    [x] generate code verifier
    [x] generate code challenge (SHA 256 hash) (+ base64 encode?)
    [x] Host web server on esp32 to display auth link to user
    [x] Generate auth link, serve on esp32 web page, get user to click
    [x] Copy auth code from above into box on webpage, use to auth this spotify sesh
    [x] Use authCode to make spotify API requests!
    [ ] Cleanup & refactor!
    */
    Serial.println();

    hf.generateCodeChallenge(); // Generate and set verifier & challenge

    // ----- wip local webserver test -----
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Configure routes
    hf.webServer.on("/", HTTP_GET, [&]() {hf.handleRoot();});
    hf.webServer.on("/auth-code", HTTP_GET, [&]() {
        hf.authorizationCode = hf.webServer.arg("code"); // Get auth code pasted by user (after successful login)
        int pos = hf.authorizationCode.indexOf("&ubi"); // Strip out other arg to get auth code
        if (pos != -1) {
            hf.authorizationCode.remove(pos);
        }

        hf.exchangeSpotifyAuthForAccess(); // Use auth code to get access code

        auto trackInfo = hf.getSpotifyCurrentPlayingSong();
        // Debug logging (remove)
        Serial.print("Song: ");
        Serial.println(trackInfo.songName);
        Serial.print("Artist: ");
        Serial.println(trackInfo.artist);
        // Debug logging (remove)
    });

    // Start server
    hf.webServer.begin();

    Serial.println("Web server started");
    // ----- wip local webserver test -----
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
