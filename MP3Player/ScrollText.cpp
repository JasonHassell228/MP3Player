#include <ScrollText.h>

// Draw text with scrolling functionality (MUST HANDLE CLEAR & DISPLAY OUTSIDE THIS FUNC)
void ScrollText::drawTextWithScroll(String &text, int yPos, unsigned long &currentTime, SSD1306Wire &display)
{
    auto totalWidth = display.getStringWidth(text);
    if (totalWidth > screenWidth)
    {
        if (scrollIndex == 0)
        { // Delay start of scroll so user has time to read

        }

        if (finishedScroll)
        {
            if (currentTime >= scrollDelayTime)
            {
                scrollIndex = screenWidth; // Reset scroll
                finishedScroll = false;
            }
        }
        else
        {
            if ((currentTime - lastScrollTime) >= 30)
            {
                lastScrollTime = currentTime;
                scrollIndex--;
                if (scrollIndex < (-totalWidth + screenWidth - 10))
                {
                    scrollDelayTime = currentTime + 3000; // 3s delay after scroll finish
                    finishedScroll = true;
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
