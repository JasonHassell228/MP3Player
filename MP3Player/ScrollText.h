#ifndef ScrollText_h
#define ScrollText_h

#include "SSD1306Wire.h"

class ScrollText
{
public:
	// UI text state
	int screenWidth = 128;
	int scrollIndex = 0;
	// Track loop timings
	bool scrollPaused = false;
	bool newScroll = true;
	unsigned long lastScrollTime = 0;
	unsigned long scrollDelayTime = 0;

	// UI functions
	void drawTextWithScroll(String &text, int yPos, unsigned long &currentTime, SSD1306Wire &display);
	void reset();

private:
	bool checkScrollFinished(int totalWidth);
};

#endif