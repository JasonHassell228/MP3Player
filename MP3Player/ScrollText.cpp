#include <ScrollText.h>

// Draw text with scrolling functionality (MUST HANDLE CLEAR & DISPLAY OUTSIDE THIS FUNC)
void ScrollText::drawTextWithScroll(String &text, int yPos, unsigned long &currentTime, SSD1306Wire &display)
{
    auto totalWidth = display.getStringWidth(text);
    if (totalWidth > screenWidth)
    {
        if (finishedScroll)
        {
            if (currentTime > scrollFinishTimeUIText)
            {
                scrollIndexUIX = screenWidth; // Reset scroll
                finishedScroll = false;
            }
        }
        else
        {
            if ((currentTime - lastScrollTimeUIText) >= 30)
            {
                lastScrollTimeUIText = currentTime;
                display.drawString(scrollIndexUIX, yPos, text); // TODO NEED TO DRAW EVERY LOOP NOT ONLY ON LOOP RESET
                scrollIndexUIX--;
                if (scrollIndexUIX < (-totalWidth + screenWidth - 10))
                {
                    scrollFinishTimeUIText = currentTime + 3000; // 3s delay after scroll finish
                    finishedScroll = true;
                }
            }
        }
    }
}
