#pragma once

#include "wled.h"

extern bool isTodayInDateRange(byte monthStart, byte dayStart, byte monthEnd, byte dayEnd);

class UsermodGroWLED : public Usermod
{
private:
    unsigned long lastTime = 0;
    bool usermodActive = true;

public:
    void setup()
    {
    }

    void loop()
    {
        if (millis() - lastTime > 60000 && usermodActive)
        { // Update every minute
            uint32_t lastTime = millis();

            // Get current time in minutes since midnight
            uint32_t currentTime = hour(localTime) * 60 + minute(localTime);

            int8_t latestMacroIndex = -1;
            bool foundMacro = false;
            uint32_t nextTimeDiff = UINT32_MAX;

            for (uint8_t i = 0; i < 8; i++)
            {
                if (timerMacro[i] != 0 &&
                    (timerWeekday[i] & 0x01) &&                           // Timer is enabled
                    ((timerWeekday[i] >> weekdayMondayFirst()) & 0x01) && // Timer should activate at current day of week
                    isTodayInDateRange(((timerMonth[i] >> 4) & 0x0F), timerDay[i], timerMonth[i] & 0x0F, timerDayEnd[i]))
                {

                    uint32_t timerTime = timerHours[i] * 60 + timerMinutes[i];

                    // Calculate time difference considering the previous occurrence
                    uint32_t timeDiff;
                    if (timerTime <= currentTime)
                    {
                        timeDiff = currentTime - timerTime; // Previous event today
                    }
                    else
                    {
                        timeDiff = (currentTime + 1440) - timerTime; // Previous event was technically yesterday
                    }

                    // Select the macro with the minimum positive time difference
                    if (timeDiff < nextTimeDiff)
                    {
                        nextTimeDiff = timeDiff;
                        latestMacroIndex = i;
                        foundMacro = true;
                    }
                }
            }

            if (foundMacro)
            {
                unloadPlaylist();
                applyPreset(timerMacro[latestMacroIndex]);
            }
        }
    }

    void addToConfig(JsonObject &root)
    {
        JsonObject top = root.createNestedObject("groWLED usermod");
        top["active"] = usermodActive;
    }

    bool readFromConfig(JsonObject &root)
    {
        // default settings values could be set here (or below using the 3-argument getJsonValue()) instead of in the class definition or constructor
        // setting them inside readFromConfig() is slightly more robust, handling the rare but plausible use case of single value being missing after boot (e.g. if the cfg.json was manually edited and a value was removed)

        JsonObject top = root["groWLED usermod"];

        bool configComplete = !top.isNull();

        configComplete &= getJsonValue(top["active"], usermodActive);

        return configComplete;
    }
};