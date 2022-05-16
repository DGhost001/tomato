#include "menu.h"
#include "ansi.h"
#include "pump.h"
#include "sensor.h"
#include "uptime.h"
#include "settings.h"

#include <stdlib.h>

extern char const * const VLine;
extern char const * const BottomLeft;
extern char const * const HLine;
extern int thresholds_[4];

static void drawHeader( void )
{
    setForegroundColor(White, false);
    setBackgroundColor(Blue);
    drawWindow(1,1,78,2, nullptr);

    printxy(22,2,"Arduino Water Controller Version 1.0");
    printxy(28,3, "(C) Falk Ahlendorf 2021");
}

static int drawLeftMenu( void )
{
    char const * const menu[] = {
      "Pump 1",
      "Pump 2",
      "Pump 3",
      "Pump 4",
      "Update",
      "Restore",
      "Save",
      "Exit",
      0
    };

    setForegroundColor(White, false);
    setBackgroundColor(Blue);

    drawWindow(1,5,9,9,"Menu");
    return selectMenu(3,7,menu,false);
}

static void drawStatusBar( void )
{
    gotoxy(1,24);

    setForegroundColor(Black, false);
    setBackgroundColor(White);

    for(unsigned i=0; i< 80; ++i)
        print(" ");

    gotoxy(1,24);

    print("Status: ");
    for(unsigned i=0; i<4; ++i) {
        if(pumpStatus(i))
        {
            setInverse(true);
        } else {
            setInverse(false);
        }

        print("P%i",i);
        setInverse(false);
        print(" ");
    }

    print("| S1: %d S2: %d S3: %d S4: %d |",
          getMesurement(0,0),
          getMesurement(1,0),
          getMesurement(2,0),
          getMesurement(3,0)
          );

    gotoxy(60,24);

    unsigned minutes, hours, seconds;

    TimeStamp now = getCurrentTime();

    hours = static_cast<unsigned>(now / 1000ull / 60ull / 60ull);
    now -= static_cast<uint64_t>(hours) * 1000ull * 60ull * 60ull;
    minutes = now /1000ull / 60ull;
    now -= static_cast<uint64_t>(minutes) * 1000ull * 60ull;
    seconds = now / 1000ull;

    print("Uptime: %d:%d:%d", hours, minutes, seconds);

}

static void drawGraph(unsigned const x, unsigned const y, unsigned const pump, unsigned threshold)
{
    /* Draw the graph */
    setForegroundColor(White, true);
    for(unsigned i=0; i< 10; ++i)
    {
        printxy(x+4, y+1+i,VLine);
    }

    printxy(x+4,y+11, BottomLeft);

    for(unsigned i=0; i<50;i++)
    {
        printxy(x+5+i,y+11, HLine);
    }

    printxy(x+4,y,"⮝");
    printxy(x+55,y+11,"➤");

    printxy(x,y+1,"100-");
    printxy(x,y+6," 50-");

    /* Draw the bars */

    setForegroundColor(Yellow, false);
    setInverse(true);

    for(unsigned i=0; i<16; ++i) {
        int measure = getMesurement(pump,i);
        measure = (measure * 5) / 512;

        for(unsigned j=0; j<measure; ++j)
        {
            printxy(x+5+i*3+1,y+10-j,"  ");
        }
    }

    setInverse(false);
    setForegroundColor(White, false);

    threshold = threshold * 5 /512;

    printxy(x+57, y+10-threshold, "← ON");


}

static int drawPumpWindow(unsigned const pump)
{
    char const * const menu[] = {
      "Set On Time",
      "Set Off Time",
      "Set Threshold",
      "Toggle On/Off",
      "Back",
      0
    };

    setForegroundColor(White, false);
    setBackgroundColor(Blue);

    drawWindow(12,5,79-12,21-4,"Pump Control");
    setForegroundColor(White, true);
    drawGraph(15,6,pump,thresholds_[pump]);
    setForegroundColor(White, false);

    printxy(42,18,"On Time [sec] : %d",pumpGetTurnOnTime(pump));
    printxy(42,19,"Off Time [sec]: %d",pumpGetTurnOffTime(pump));
    printxy(42,20,"Threshold [DN]: %d",thresholds_[pump]);


    return selectMenu(15,18,menu, false);
}

void enterMenu( void )
{
    setForegroundColor(White, false);
    setBackgroundColor(Black);
    setInverse(false);

    clearScreen();

    drawHeader();

    unsigned mainMenu = 0;
    unsigned pumpMenu = 4;
    while(mainMenu != 7) {
        drawStatusBar();
        if(4 == pumpMenu )
        {
            mainMenu = drawLeftMenu();
            drawStatusBar();
        }

        if(mainMenu < 4)
        {
            pumpMenu = drawPumpWindow(mainMenu);
            drawStatusBar();

            switch (pumpMenu) {
            case 0: {
                char buffer[6];
                setForegroundColor(White, false);
                setBackgroundColor(Green);
                getString(31,11,"On Time: ", buffer,6);
                pumpSetTurnOnTime(mainMenu, atoi(buffer));
            } break;
            case 1: {
                char buffer[6];
                setForegroundColor(White, false);
                setBackgroundColor(Green);
                getString(31,11,"Off Time: ", buffer,6);
                pumpSetTurnOffTime(mainMenu, atoi(buffer));
            }break;

            case 2: {
                char buffer[6];
                int tmp;
                setForegroundColor(White, false);
                setBackgroundColor(Green);
                getString(31,11,"Threshold: ", buffer,5);
                tmp = atoi(buffer);

                if(tmp < 0) tmp = 0;
                if(tmp > 1023) tmp = 1023;

                thresholds_[mainMenu] = tmp;
            }break;
            case 3: {
                pumpToggleOnOff(mainMenu);
            }break;
            case 4: {
                setForegroundColor(Black, false);
                setBackgroundColor(Black);
                drawWindow(12,5,79-12,22-4,nullptr);
            } break;

            default:
                break;
            }



            if(4 == pumpMenu) {
            }
        } else if(4 == mainMenu) {
            updateSensors();
        } else if(5 == mainMenu) {
            setForegroundColor(White, true);
            setBackgroundColor(Red);

            Button b = showMessage(21,11,"Confirm", "Do you want to reload the values from EEPROM?", static_cast<Button>(BUTTON_OK | BUTTON_CANCLE));
            setForegroundColor(Black, false);
            setBackgroundColor(Black);
            drawWindow(21,11,50,5,nullptr);

            if(BUTTON_OK == b) {
                Settings *s = loadSettings();

                if(s) //Only load settings if they are valid
                {
                    for(unsigned i = 0; i< 4; ++i){
                        pumpSetTurnOnTime(i,s->pumpSettings[i].turnOnTime);
                        pumpSetTurnOffTime(i,s->pumpSettings[i].turnOffTime);
                        thresholds_[i] = s->pumpSettings[i].threshold;
                    }

                    setForegroundColor(White, false);
                    setBackgroundColor(Green);

                    showMessage(21,11,"ok", "Settings loaded successfully!", static_cast<Button>(BUTTON_OK));

                    setForegroundColor(Black, false);
                    setBackgroundColor(Black);
                    drawWindow(21,11,50,5,nullptr);

                } else {
                    setForegroundColor(White, false);
                    setBackgroundColor(Red);

                    showMessage(21,11,"Failed", "Settings loading failed!", static_cast<Button>(BUTTON_OK));

                    setForegroundColor(Black, false);
                    setBackgroundColor(Black);
                    drawWindow(21,11,50,5,nullptr);

                }
            }


        }  else if(6 == mainMenu) {
            setForegroundColor(White, true);
            setBackgroundColor(Red);
            Button b = showMessage(31,11,"Confirm", "  Save values to EEPROM?", static_cast<Button>(BUTTON_OK | BUTTON_CANCLE));
            setForegroundColor(Black, false);
            setBackgroundColor(Black);
            drawWindow(21,11,50,5,nullptr);

            if(BUTTON_OK == b) {
                Settings *s = newSettings();

                if(s) //Only load settings if they are valid
                {
                    for(unsigned i = 0; i< 4; ++i){
                        s->pumpSettings[i].turnOnTime = pumpGetTurnOnTime(i);
                        s->pumpSettings[i].turnOffTime = pumpGetTurnOffTime(i);
                        s->pumpSettings[i].threshold = thresholds_[i];
                    }

                    saveSettings();
                    setForegroundColor(White, false);
                    setBackgroundColor(Green);

                    showMessage(21,11,"ok", "Settings saved successfully!", static_cast<Button>(BUTTON_OK));

                    setForegroundColor(Black, false);
                    setBackgroundColor(Black);
                    drawWindow(21,11,50,5,nullptr);

                } else {
                    setForegroundColor(White, false);
                    setBackgroundColor(Red);

                    showMessage(21,11,"Failed", "Settings saving failed!", static_cast<Button>(BUTTON_OK));

                    setForegroundColor(Black, false);
                    setBackgroundColor(Black);
                    drawWindow(21,11,50,5,nullptr);
                }
            }
        }
    }

    setForegroundColor(White, false);
    setBackgroundColor(Black);
    setInverse(false);

    clearScreen();

}
