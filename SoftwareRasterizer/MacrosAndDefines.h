#pragma once

// defines for windows 
#define WINDOW_CLASS_NAME "WINCLASS1"



#define BITMAP_ID 0x4D42

//COM calls need this to be defined
#define INITGUID

//returns 1 upon true, 0 upon false
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)


// this builds a 16 bit color value in 5.5.5 format (1-bit alpha mode)
#define _RGB16BIT555(r,g,b) ( (b & 31) + ((g & 31) << 5) + ((r & 31) << 10) )

// this builds a 16 bit color value in 5.6.5 format (green dominate mode)
#define _RGB16BIT565(r,g,b) ( (b & 31) + ((g & 63) << 5) + ((r & 31) << 11) )

#define _RGB24BIT888(r,g,b) ( (b & 255) + ((g & 255) << 8) + ((r & 255) << 16) )

//identical to 24 bit since 8 alpha bits aren't used
#define _RGB32BITX888(r, g, b) ( (b & 255) + ((g & 255) << 8) + ((r & 255) << 16) )

#define _RGBA32BIT(r, g, b, a) ( (r & 255) + ((g & 255) << 8) + ((b & 255) << 16)  + ((a & 255) << 24) )

// initializes a direct draw struct
#define DDRAW_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }

#define MATH_DEGTORAD(degrees) (degrees * MATH_PI/180)
#define MATH_RADTODEG(rad) (rad * 180/MATH_PI);

//colors - primary, secondary and neutral
#define COLOR_RED _RGBA32BIT(255, 0, 0, 255)
#define COLOR_GREEN _RGBA32BIT(0, 255, 0, 255)
#define COLOR_BLUE _RGBA32BIT(0, 0, 255, 255)

#define COLOR_YELLOW _RGBA32BIT(255, 255, 0, 255)
#define COLOR_CYAN	 _RGBA32BIT(0, 255, 255, 255)
#define COLOR_MAGENTA _RGBA32BIT(255, 0, 255, 255)

#define COLOR_GREY  _RGBA32BIT(125, 125, 125, 255)
#define COLOR_BLACK _RGBA32BIT(0, 0, 0, 255)
#define COLOR_WHITE _RGBA32BIT(255, 255, 255, 255)

