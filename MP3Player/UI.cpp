#include <UI.h>

UI::UI(int address, int sda, int scl) : display(address, sda, scl, GEOMETRY_128_32) {}

// ----- UI HELPER FUNCTIONS -----
void UI::initializeDisplay()
{
    // Setup
    display.init();
    display.flipScreenVertically();
    display.setFont(Monospaced_plain_8);
    display.clear();
    display.setTextAlignment(TEXT_ALIGN_LEFT);

    // Draw welcome text
    display.drawString(0, 10, "Connecting to Wifi...");
    // write the buffer to the display
    display.display();
}

void UI::drawNowPlaying(TrackInfo currentlyPlaying, unsigned long &currentTime)
{
    // Draw song & artist name UI
    Serial.print("screen should be: ");
    Serial.print(currentlyPlaying.songName);
    Serial.print(", ");
    Serial.println(currentlyPlaying.artist);
    display.clear(); // TODO clear with black rectangle to save energy & preserve header?
    display.setFont(Monospaced_plain_9);
    songName.drawTextWithScroll(currentlyPlaying.songName, 10, currentTime, display);
    display.setFont(Monospaced_plain_8);
    artist.drawTextWithScroll(currentlyPlaying.artist, 25, currentTime, display);
    display.display();
}