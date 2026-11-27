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

void UI::drawProgressBar(TrackProgressInfo progress)
{
    if (progress.isPlaying)
    {
        auto progressComplete = (float)progress.progressMs / progress.durationMs;
        int start = 3;
        int end = 124;
        int current = start + (end - start) * progressComplete;
        display.drawLine(start, 30, current, 30);
    }
}

void UI::drawNowPlaying(TrackInfo currentlyPlaying,
                        TrackProgressInfo progress,
                        unsigned long &currentTime)
{
    // Draw song & artist name UI
    display.clear(); // TODO clear with black rectangle to save energy & preserve header?

    // Display song name & artist
    display.setFont(Monospaced_plain_10);
    songName.drawTextWithScroll(currentlyPlaying.songName, 10, currentTime, display);
    display.setFont(Monospaced_plain_8);
    artist.drawTextWithScroll(currentlyPlaying.artist, 20, currentTime, display);

    // Display track progress bar
    drawProgressBar(progress);

    display.display();
}

void UI::resetNowPlayingScroll()
{
    songName.reset();
    artist.reset();
}
