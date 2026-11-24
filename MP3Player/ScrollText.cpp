#include <ScrollText.h>

// Draw text with scrolling functionality (MUST HANDLE CLEAR & DISPLAY OUTSIDE THIS FUNC)
void ScrollText::drawTextWithScroll(String &text, int yPos, unsigned long &currentTime, SSD1306Wire &display)
{
    auto totalWidth = display.getStringWidth(text);
    if (totalWidth > screenWidth)
    {
        if (newScroll)
        { // Delay start of scroll so user has time to read
            scrollDelayTime = currentTime;
            newScroll = false;
            scrollPaused = true;
        }
        else if (scrollPaused)
        {
            if (currentTime - scrollDelayTime >= 3000)
            { // 3s delay for scroll pause
                if (checkScrollFinished(totalWidth))
                {
                    scrollIndex = 0; // Reset scroll
                    newScroll = true;
                }
                scrollPaused = false;
            }
        }
        else
        {
            if ((currentTime - lastScrollTime) >= 30)
            {
                lastScrollTime = currentTime;
                scrollIndex--;
                if (checkScrollFinished(totalWidth))
                {
                    scrollDelayTime = currentTime;
                    scrollPaused = true;
                }
            }
        }
        display.drawString(scrollIndex, yPos, text);
    }
    else
    {
        display.drawString(0, yPos, text);
    }
}

bool ScrollText::checkScrollFinished(int totalWidth)
{
    return scrollIndex < (-totalWidth + screenWidth - 10);
}

// Reset scroll (called on song change)
void ScrollText::reset()
{
    scrollIndex = 0;
    newScroll = true;
    scrollPaused = false;
    scrollDelayTime = 0;
    lastScrollTime = 0;
}
