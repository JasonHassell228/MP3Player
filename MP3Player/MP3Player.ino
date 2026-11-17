// Other libs imported in HelperFunctions.h
#include <WebServer.h>
#include <WiFi.h>
#include <Wire.h>
#include "HelperFunctions.h"
#include "SSD1306Wire.h"

HelperFunctions hf;

SSD1306Wire display(0x3c, hf.SCREEN_SDA_PIN, hf.SCREEN_SCK_PIN, GEOMETRY_128_32);

// ----- wip local webserver test -----
// Create web server on port 80
WebServer server(80);

// Main webpage
void handleRoot() {
  String authLink = hf.generateSpotifyAuthURL(hf.codeChallenge);

  String html = R"rawliteral(
    <!DOCTYPE html>
    <html>
    <head>
      <title>ESP32 Test Web Server</title>
      <meta name="viewport" content="width=device-width, initial-scale=1">
    </head>
    <body>
      <h1>Spotify authentication</h1>
      <p>
        Spotify auth link:
        <a href=")rawliteral" + authLink + R"rawliteral(" target="_blank">Auth link</a>
      </p>
      <p>Here's the authorisation link. It will open a login page for Spotify in a new tab.</p>
      <p>Once you login, just copy the code parameter from the URL. It should look something like this:</p>
      <p>http://127.0.0.1:8080/?code=AQCYMmZY1PagU3rt4ji_9h8yGREfBHOZEpqEH1W204caWfeyLA70...</p>
      <p>You want the part after 'code=', make sure to strip out anything after &ubi= if it's there!</p>
      <form action="/auth-code" method="GET">
        <input name="code" placeholder="Paste Spotify auth code">
        <button type="submit">Submit</button>
      </form>
    </body>
    </html>
  )rawliteral";

  server.send(200, "text/html", html);
}
// ----- wip local webserver test -----

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
    hf.setPullupPins(hf.ALL_BTN_PINS);

    hf.keyboard.setLogLevel(HIDLogLevel::Normal);
    hf.keyboard.begin();

    initializeDisplay();

    WiFi.begin(hf.ssid, hf.password);

    // attempt to connect to Wifi network:
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        // wait 1 second for re-trying
        delay(1000);
    }

    Serial.print("Connected to ");
    Serial.println(hf.ssid);

    hf.client.setInsecure();//skip verification

    // ----- wip spotify test -----
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
    hf.codeVerifier = hf.generateCodeVerifier(64);

    String codeVerifierCStr = hf.codeVerifier.c_str();
    auto codeVerifierHashed = hf.generateSHA256(codeVerifierCStr);

    Serial.print("code verifier: ");
    Serial.println(hf.codeVerifier);
    Serial.println("code verifier SHA256: ");
    for (uint8_t byte : codeVerifierHashed) {
        Serial.print(byte);
        Serial.print(" ");
    }
    Serial.println();

    // ----- wip base64 encode test -----
    std::vector<uint8_t> data(codeVerifierHashed.begin(), codeVerifierHashed.end());
	hf.codeChallenge = hf.encode_base64(data);

	Serial.printf("Original (%d bytes): ", data.size());
	hf.printBlockAsUtf8(data);
	Serial.println();

	Serial.printf("Encoded (code challenge) (%d bytes): ", hf.codeChallenge.length());
	Serial.print(hf.codeChallenge.c_str());
	Serial.println();

    // ----- wip base64 encode test -----

    // ----- wip spotify test -----

    // ----- wip local webserver test -----
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());

    // Configure routes
    server.on("/", HTTP_GET, [&]() {handleRoot();});
    server.on("/auth-code", HTTP_GET, [&]() {
        hf.authorizationCode = server.arg("code");
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
    server.begin();

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
    // ----- wip local webserver test -----
    server.handleClient();
    // ----- wip local webserver test -----

    delay(50);
}
