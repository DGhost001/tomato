#ifndef ANSI_H
#define ANSI_H

#include <stdbool.h>
#include <stdint.h>

typedef enum
{
  Black = 0,
  Red = 1,
  Green = 2,
  Yellow = 3,
  Blue = 4,
  Magenta = 5,
  Cyan = 6,
  White = 7
}Colors;


typedef enum
{
    normal = 5,
    double_hight_top = 3,
    double_hight_bottom = 4,
    double_width = 6
}LineLayout;

typedef enum
{
    setukg0,
    setukg1,
    setusg0,
    setusg1,
    setspecg0,
    setspecg1,
    setaltg0,
    setaltg1,
    setaltspecg0,
    setaltspecg1
}CharacterSet;

enum
{
    KEY_UP    = 23361,
    KEY_LEFT  = 23363,
    KEY_DOWN  = 23362,
    KEY_RIGHT = 23364,
    KEY_RETURN= 13,
    KEY_DEL   = 0x7f,
    KEY_BACKSPACE = 0x08
};

typedef enum
{
    BUTTON_OK = 1,
    BUTTON_CANCLE = 2
}Button;

    extern void ultostr(unsigned long val, char *const s );
    extern bool hextoi(const char *buffer, unsigned int *const i);

    extern void print(const char * const format,...);
    extern void printxy(const unsigned int x, const unsigned int y, const char *const format,...);
    extern void clearScreen(void);
    extern void gotoxy(const unsigned int x, const unsigned int y);
    extern void gotox(const unsigned int x);
    extern void removeChar(void);
    extern void setForegroundColor(const Colors color, const bool bold);
    extern void setBackgroundColor(const Colors color);
    extern void setLineLayout(const LineLayout);
    extern void resetColor(void);
    extern void selectCharacterSet(const CharacterSet set);
    void setInverse(const bool);
    extern void drawWindow(const unsigned int x, const unsigned int y, const uint_fast8_t w, const uint_fast8_t h, const char *const title);
    extern void drawMenu(const unsigned int x, const unsigned int y, const unsigned int sel, const char *const items[], const bool horizontal);
    extern unsigned int selectMenu(const unsigned int x, const unsigned int y, const char *const items[],const bool horizontal);
    extern void getString(const unsigned int x, const unsigned int y, const char *const text, char *const buffer, const unsigned int length);
    extern Button showMessage(unsigned int x, unsigned int y, const char* const title, const char* const text, Button buttons);


    extern void saveCursor(void);
    extern void restoreCursor(void);
#endif // ANSI_H
