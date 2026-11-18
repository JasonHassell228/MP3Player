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
    hf.server.on("/", HTTP_GET, [&]() {hf.handleRoot();});
    hf.server.on("/auth-code", HTTP_GET, [&]() {
        hf.authorizationCode = hf.server.arg("code");
        Serial.println("authorizationCode before: ");
        Serial.println(hf.authorizationCode);
        int pos = hf.authorizationCode.indexOf("&ubi");
        if (pos != -1) {
            hf.authorizationCode.remove(pos);
        }
        Serial.println("authorizationCode after: ");
        Serial.println(hf.authorizationCode);

        // ----- wip Spotify exchange auth for access code test -----
        String accessToken = "";
        String body =
          "grant_type=authorization_code"
          "&code=" + hf.authorizationCode +
          "&redirect_uri=" + String(hf.spotify_redirect_uri) +
          "&client_id=" + hf.client_id +
          "&code_verifier=" + hf.codeVerifier;

        hf.client.connect("accounts.spotify.com", 443);

        hf.client.println("POST /api/token HTTP/1.1");
        hf.client.println("Host: accounts.spotify.com");
        hf.client.println("Content-Type: application/x-www-form-urlencoded");
        hf.client.println("Content-Length: " + String(body.length()));
        hf.client.println("Connection: close");
        hf.client.println();
        hf.client.print(body);

        Serial.println("body: ");
        Serial.println(body);
        Serial.print("finished printing body: ");

        String response;
        while (hf.client.connected() || hf.client.available()) {
            if (hf.client.available()) response += hf.client.readString();
        }

        Serial.println("response: ");
        Serial.println(response);

        JsonDocument doc;
        deserializeJson(doc, response.substring(response.indexOf("\r\n\r\n"))); // Find HTTP header separator
        accessToken = doc["access_token"].as<String>();

        Serial.println("FINAL ACCESS TOKEN: ");
        Serial.println(accessToken);
        hf.client.stop();
        // ----- wip Spotify exchange auth for access code test -----

        // // ----- wip Spotify API test -----
        if (!hf.client.connect("api.spotify.com", 443)) {
          Serial.println("Spotify connection failed 443");
        }

        hf.client.println("GET /v1/me/player/currently-playing HTTP/1.1");
        hf.client.println("Host: api.spotify.com");
        hf.client.println("Authorization: Bearer " + accessToken);
        hf.client.println("Connection: close");
        hf.client.println();

        response = "";

        while (hf.client.connected() || hf.client.available()) {
            if (hf.client.available()) {
                response += hf.client.readString();
            }
        }
        Serial.println("second response: ");
        Serial.println(response);

        deserializeJson(doc, response.substring(response.indexOf("\r\n\r\n"))); // Find HTTP header separator
        String song = doc["item"]["name"].as<String>();
        String artist = doc["item"]["artists"][0]["name"].as<String>();

        Serial.println(song);
        Serial.println(artist);

        hf.client.stop();
        // ----- wip Spotify API test -----
    });

    // Start server
    hf.server.begin();

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
    hf.server.handleClient();

    delay(50);
}
