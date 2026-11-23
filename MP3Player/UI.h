#ifndef UI_h
#define UI_h

#include <Arduino.h>
#include <ScrollText.h>
#include <UIData.h>

struct TrackInfo
{
	String songName;
	String artist;
};

class UI
{
public:
	SSD1306Wire display; // UI object

	ScrollText songName;
	ScrollText artist;

	UI(int address, int sda, int scl);

	// UI functions
	void initializeDisplay();
	void drawNowPlaying(TrackInfo currentlyPlaying, unsigned long &currentTime);
};

#endif