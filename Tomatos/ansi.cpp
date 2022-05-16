#include "ansi.h"
#include <stdarg.h>
#include <Arduino.h>
#include <HardwareSerial.h>

extern HardwareSerial Serial;


static char const * const CSI = "\x1b[";
extern char const * const TopLeft  = "┌";
extern char const * const TopRight = "┐";
extern char const * const BottomLeft = "└";
extern char const * const BottomRight = "┘";
extern char const * const HLine    = "─";
extern char const * const VLine    = "│";

static char const * const okButton = "<OK>";
static char const * const caButton = "<Cancle>";

static unsigned long const itoa_lookupTable[] =
{
          10,
         100,
        1000,
//     65535
       10000,
      100000,
     1000000,
    10000000,
   100000000,
//4294967296
  1000000000,
};

static char hexlookupTable[] = {'0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};

static void writeHex(uint8_t value)
{
   Serial.print(hexlookupTable[(value >> 4)&0xf]);
   Serial.print(hexlookupTable[value & 0xf]);
}

void ultostr(unsigned long val, char* const s )
{
  char *p;
  uint_fast8_t d, i;
  bool isLeadingZero;
  unsigned long uval = val;

  p = s;

  isLeadingZero = true;
  i = val<100000?9:4;
  do{
    --i;

    for( d = '0'; uval >= itoa_lookupTable[i]; uval -= itoa_lookupTable[i],++d )
    {
      isLeadingZero = false;
    }

    if( !isLeadingZero )
    {
      *p++ = d ;
    }
  }while( i );

  *p++ = (unsigned char)uval + '0';
  *p++ = 0; //Zero Terminator
}

static void vprint(const char* const format,va_list va)
{
    const char* pos;
    char tmp[16];
    unsigned int tmpi;

    pos = format;
    while((*pos))
    {
        if(*pos == '%')
        {
            ++pos;
            switch(*pos)
            {
            case 0: Serial.print('%'); continue;
            case 's': Serial.print(va_arg(va, char*)); break;
            case 'c': Serial.print((char)va_arg(va,int)); break;
            case 'u':
            case 'd':
                ultostr(va_arg(va, unsigned int),tmp);
                Serial.print(tmp);
                break;
            case 'i':
                tmpi = va_arg(va, unsigned int);
                if(tmpi & 0x8000)
                {
                    Serial.print('-');
                    tmpi =(~tmpi)+1;
                }
                ultostr(tmpi,tmp);
                Serial.print(tmp);
                break;
            case 'x':
                tmpi = va_arg(va, unsigned int);
                writeHex(tmpi>>8);
                writeHex(tmpi);
                break;
            case '%': Serial.print('%'); break;
            default: Serial.print('%'); Serial.print(*pos); break;
            }
        }else
        {
            Serial.print(*pos);
        }
        ++pos;
    }
}

void print(const char *const format,...)
{
    va_list va;
    va_start(va,format);
    vprint(format,va);
    va_end(va);
}

void printxy(const unsigned int x, const unsigned int y, const char *const format,...)
{
    va_list va;
    va_start(va,format);
    gotoxy(x,y);
    vprint(format,va);
    va_end(va);
}

static void waitForKeypressed()
{
    while(Serial.available() == 0) { }
}

unsigned int readKey(void)
{
    char c1;
    char c2 = 0;

    waitForKeypressed();
    c1 = Serial.read();
    if(c1 == 0x1b)
    {
        waitForKeypressed();
        c2 = Serial.read();
        waitForKeypressed();
        while(Serial.available()>0)
        {
            c1 = Serial.read();
        }
    }
    return (c2<<8) | c1;
}


void clearScreen()
{
    Serial.print(CSI);
    Serial.print("?6h");
    Serial.print("\x1b[2J\x1b[1;1H");
}

void drawWindow(const unsigned int x,const unsigned int y,const uint_fast8_t w,const uint_fast8_t h, const char* const title)
{
    uint_fast8_t k;
    uint_fast8_t i;
    gotoxy(x,y);
    Serial.print("\x1b%G");
    Serial.print(TopLeft);
    for(i=0;i<w;++i)
    {
        Serial.print(HLine);
    }

    Serial.print(TopRight);

    for(k=0; k<h; ++k)
    {
        gotoxy(x,y+k+1);
        Serial.print(VLine);
        for(i=0;i<w;++i)
        {
            Serial.print(' ');
        }
        Serial.print(VLine);
    }
    gotoxy(x,y+h+1);
    Serial.print(BottomLeft);

    for(i=0; i<w;++i)
    {
        Serial.print(HLine);
    }

    Serial.print(BottomRight);

    Serial.print("\x1b%@");

    if(title)
    {
        gotoxy(x+w/2-((strlen(title)+2)/2),y);
        Serial.print('[');
        Serial.print(title);
        Serial.print(']');
    }
    gotoxy(x+1,y+1);
}

void drawMenu(const unsigned int x, const unsigned int y,const unsigned int sel, char const* const items[], const bool horizontal)
{
    uint_fast8_t citem=0;
    while((items[citem])!=0)
    {
        if(horizontal && citem != 0)
        {
            Serial.print('\t');
        }else
        {
            gotoxy(x,y+citem);
        }
        if(sel == citem)
        {
            setInverse(true);
            Serial.print(items[citem]);
        }else
        {
            setInverse(false);
            Serial.print(items[citem]);
        }
        ++citem;
    }
}

unsigned int selectMenu(const unsigned int x, const unsigned int y,char const * const items[],const bool horizontal)
{
    unsigned int keyCode = 0;
    uint_fast8_t sel = 0;

    while(keyCode != KEY_RETURN)
    {
        drawMenu(x,y,sel,items,horizontal);
        keyCode = readKey();

        switch(keyCode)
        {
        case KEY_UP:
        case KEY_LEFT:
        {
            if (sel == 0)
            {
                while(items[sel] != 0) ++sel;
                --sel;
            }else
            {
                --sel;
            }
            break;
        }
        case KEY_DOWN:
        case KEY_RIGHT:
        {
            if(items[sel] == 0)
            {
                sel = 0;
            }else
            {
                ++sel;
                if(items[sel] == 0)
                {
                    sel = 0;
                }
            }
            break;
        }
        }
    }

    resetColor();
    return sel;
}


void setInverse(const bool enable)
{
    Serial.print(CSI);
    if(enable)
    {
        Serial.print("7m");
    }else
    {
        Serial.print("27m");
    }
}

void gotoxy(const unsigned int x, const unsigned int y)
{
    char tmpx[6];
    char tmpy[6];

    ultostr(x,tmpx);
    ultostr(y,tmpy);
    Serial.print(CSI);
    Serial.print(tmpy);
    Serial.print(';');
    Serial.print(tmpx);
    Serial.print('H');
}

void gotox(const unsigned int x)
{
    char tmpx[6];
    ultostr(x,tmpx);
    Serial.print(CSI);
    Serial.print(tmpx);
    Serial.print('G');
}

void removeChar(void)
{
    Serial.print("\x1b[P");
}

void setForegroundColor(const Colors color, const bool bold)
{
  char tmpcl[3];
  ultostr(color+30,tmpcl);

  Serial.print(CSI);
  Serial.print(tmpcl);

  if(bold == true)
  {
      Serial.print(";1m");
  }else
  {
      Serial.print('m');
  }
}

void setLineLayout(const LineLayout layout)
{
    Serial.print("\x1b#");
    Serial.print('0'+layout);
}


void setBackgroundColor(const Colors color)
{
  char tmpcl[3];
  ultostr(color+40,tmpcl);

  Serial.print(CSI);
  Serial.print(tmpcl);
  Serial.print('m');
}

void resetColor(void)
{
  Serial.print("\x1b[0m");
}

void selectCharacterSet(const CharacterSet set)
{
    Serial.print(0x1b);

    switch(set)
    {
    case setukg0:   Serial.print("(A"); break;
    case setukg1:   Serial.print(")A"); break;
    case setusg0:   Serial.print("(B"); break;
    case setusg1:   Serial.print(")B"); break;
    case setspecg0: Serial.print("(0"); break;
    case setspecg1: Serial.print(")0"); break;
    case setaltg0:  Serial.print("(1"); break;
    case setaltg1:  Serial.print(")1"); break;
    case setaltspecg0: Serial.print("(2"); break;
    case setaltspecg1: Serial.print(")2"); break;
    default: Serial.print("(B");
    }
}

void saveCursor()
{
    Serial.print("\x1b");
    Serial.print('7');
}

void restoreCursor()
{
    Serial.print("\x1b");
    Serial.print('8');
}

void getString(const unsigned int x, const unsigned int y, char const*const text, char*const buffer, const unsigned int length)
{
    const unsigned int txtLeng = strlen(text);
    unsigned int key = 0;
    unsigned int pos;

    for(pos=0; pos<length; ++pos)
    {
        buffer[pos] = 0;
    }
    pos = 0;

    drawWindow(x,y,txtLeng+length,1,"Input");
    print("%s",text);

    while(key != KEY_RETURN)
    {
        key = readKey();
        if((key > ' ' && key < 0x7F) && pos < length-1 )
        {
            buffer[pos] = key;
            ++pos;
            print("%c",key);
        }

        if ((key == KEY_BACKSPACE | key == KEY_DEL ) && pos > 0)
        {
            buffer[pos] = 0;
            --pos;
            printxy(x+txtLeng+1,y+1,"%s ",buffer);
            gotoxy(x+txtLeng+1+pos,y+1);
        }
    }
}

Button showMessage(unsigned int x, unsigned int y, const char *const title, const char *const text, Button buttons)
{
    char const * menuItem[3];
    const unsigned int txtLeng = strlen(text);
    const unsigned int titleLeng = strlen(title);
    unsigned int i= 0;
    unsigned int windowWidth = txtLeng + 2;

    if(buttons & BUTTON_OK)
    {
        menuItem[i++] = okButton;
    }

    if(buttons & BUTTON_CANCLE)
    {
        menuItem[i++] = caButton;
    }

    menuItem[i] = 0;


    if(windowWidth < titleLeng)
    {
        windowWidth = titleLeng +2;
    }

    drawWindow(x,y,windowWidth,2,title);
    printxy(++x,++y,"%s",text);
    i = selectMenu(x+windowWidth/2-buttons*2,++y,menuItem,true);

    return (menuItem[i] == okButton) ? BUTTON_OK : BUTTON_CANCLE;
}

bool hextoi(const char *buffer, unsigned int *const i)
{
    unsigned int result=0;

    while(*buffer != 0)
    {
        if(*buffer == ' ')
        {

        }else if (*buffer >= '0' && *buffer <='9')
        {
            result=result<<4;
            result|=(*buffer)-'0';
        }else if (*buffer >= 'A' && *buffer <='F')
        {
            result=result<<4;
            result|=(*buffer)-'A'+10;
        }else if (*buffer >= 'a' && *buffer <='f')
        {
            result=result<<4;
            result|=(*buffer)-'a'+10;
        }else
        {
            return false;
        }

        ++buffer;
    }

    (*i) = result;

    return true;
}
