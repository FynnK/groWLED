#pragma once

#include "wled.h"

extern bool isTodayInDateRange(byte monthStart, byte dayStart, byte monthEnd, byte dayEnd);

class UsermodGroWLED : public Usermod {
private:
    unsigned long lastTime = 0;

public:
    void setup() {
        // Your setup code goes here
    }

    void loop() {
        if (millis() - lastTime > 60000) { // Update every minute
            lastTime = millis();

            // Get current time
            uint8_t currentHour = hour(localTime);
            uint8_t currentMinute = minute(localTime);
            uint8_t latestMacroIndex = -1;
            bool foundMacro = false;

            for (uint8_t i = 0; i < 8; i++){
              if (timerMacro[i] != 0
                  && (timerWeekday[i] & 0x01) //timer is enabled
                  && (timerHours[i] <= hour(localTime)) //if hour is set to 24, activate every hour
                  && timerMinutes[i] <= minute(localTime)
                  && ((timerWeekday[i] >> weekdayMondayFirst()) & 0x01) //timer should activate at current day of week
                  && isTodayInDateRange(((timerMonth[i] >> 4) & 0x0F), timerDay[i], timerMonth[i] & 0x0F, timerDayEnd[i])
                ){
                  foundMacro = true;
                  latestMacroIndex = i;
                }
              }

            if(foundMacro){
              unloadPlaylist();
              applyPreset(timerMacro[latestMacroIndex]);
            }
            foundMacro = false;
        }
    }
};