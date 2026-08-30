const int playPauseButtonPin = 19;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE Work!");
  pinMode(playPauseButtonPin, INPUT_PULLUP);
}

void loop() {
    Serial.println("Connected to BT!!");
    delay(1000);

    Serial.println("Sending enter..");
    delay(1000);

    Serial.println("Sending play/pause..");

    delay(1000);

  delay(200);
}
