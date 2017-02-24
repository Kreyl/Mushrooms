/*
 * color.h
 *
 *  Created on: 05 ���. 2014 �.
 *      Author: Kreyl
 */

#pragma once

#include "inttypes.h"
#include <sys/cdefs.h>
#include "uart.h"

// Mixing two colors
#define ClrMix(C, B, L)     ((C * L + B * (255 - L)) / 255)

// Smooth delay
static inline uint32_t ClrCalcDelay(uint32_t AValue, uint32_t Smooth) {
    return (uint32_t)((Smooth / (AValue+4)) + 1);
}

struct Color_t {
    union {
        uint32_t DWord32;
        struct {
            uint8_t R, G, B;
        };
    };
    bool operator == (const Color_t &AColor) const { return (DWord32 == AColor.DWord32); }
    bool operator != (const Color_t &AColor) const { return (DWord32 != AColor.DWord32); }
    Color_t& operator = (const Color_t &Right) { DWord32 = Right.DWord32; return *this; }
    void Adjust(const Color_t &PColor) {
        if     (R < PColor.R) R++;
        else if(R > PColor.R) R--;
        if     (G < PColor.G) G++;
        else if(G > PColor.G) G--;
        if     (B < PColor.B) B++;
        else if(B > PColor.B) B--;
    }
    void Adjust(const Color_t &PColor, uint32_t Step) {
        uint32_t ThrsR = 255 - Step;
        if(R < PColor.R) {
            if(R <= ThrsR) R += Step;
            else R = 255;
        }
        else if(R > PColor.R) {
            if(R >= Step) R -= Step;
            else R = 0;
        }

        if(G < PColor.G) {
            if(G <= ThrsR) G += Step;
            else G = 255;
        }
        else if(G > PColor.G) {
            if(G >= Step) G -= Step;
            else G = 0;
        }

        if(B < PColor.B) {
            if(B <= ThrsR) B += Step;
            else B = 255;
        }
        else if(B > PColor.B) {
            if(B >= Step) B -= Step;
            else B = 0;
        }
    }
    void FromRGB(uint8_t Red, uint8_t Green, uint8_t Blue) { R = Red; G = Green; B = Blue; }
    void ToRGB(uint8_t *PR, uint8_t *PG, uint8_t *PB) const { *PR = R; *PG = G; *PB = B; }
    bool IsEqualRGB(uint8_t Red, uint8_t Green, uint8_t Blue) { return (R == Red and G == Green and B == Blue); }
    void SetSameRGB(const Color_t &AColor) { R = AColor.R; G = AColor.G; B = AColor.B; }
    uint8_t RGBTo565_HiByte() const {
        uint32_t rslt = R & 0b11111000;
        rslt |= G >> 5;
        return (uint8_t)rslt;
    }
    uint8_t RGBTo565_LoByte() const {
        uint32_t rslt = (G << 3) & 0b11100000;
        rslt |= B >> 3;
        return (uint8_t)rslt;
    }
    uint16_t RGBTo565() const {
        uint16_t rslt = ((uint16_t)(R & 0b11111000)) << 8;
        rslt |= ((uint16_t)(G & 0b11111100)) << 3;
        rslt |= ((uint16_t)B) >> 3;
        return rslt;
    }
    void BeMixOf(const Color_t &Fore, const Color_t &Back, uint32_t Brt) {
        R = ClrMix(Fore.R, Back.R, Brt);
        G = ClrMix(Fore.G, Back.G, Brt);
        B = ClrMix(Fore.B, Back.B, Brt);
    }
    uint32_t DelayToNextAdj(const Color_t &AClr, uint32_t SmoothValue) {
        uint32_t Delay, DelayTmp;
        Delay = (R == AClr.R)? 0 : ClrCalcDelay(R, SmoothValue);
        DelayTmp = (G == AClr.G)? 0 : ClrCalcDelay(G, SmoothValue);
        if(DelayTmp > Delay) Delay = DelayTmp;
        DelayTmp = (B == AClr.B)? 0 : ClrCalcDelay(B, SmoothValue);
        if(DelayTmp > Delay) Delay = DelayTmp;
        return Delay;
    }
    void Print() { Uart.Printf("{%u, %u, %u}", R, G, B); }
    void PrintI() { Uart.PrintfI("{%u, %u, %u}", R, G, B); }
    Color_t() : R(0), G(0), B(0) {}
    Color_t(uint8_t AR, uint8_t AG, uint8_t AB) : R(AR), G(AG), B(AB) {}
} __attribute__((packed));


// ============================ Common methods =================================
#define RED_OF(c)           (((c) & 0xF800)>>8)
#define GREEN_OF(c)         (((c)&0x007E)>>3)
#define BLUE_OF(c)          (((c)&0x001F)<<3)

static inline uint16_t RGBTo565(uint16_t r, uint16_t g, uint16_t b) {
    uint16_t rslt = (r & 0b11111000) << 8;
    rslt |= (g & 0b11111100) << 3;
    rslt |= b >> 3;
    return rslt;
}

// Blend two colors according to the alpha;
// The alpha value (0-255). 0 is all background, 255 is all foreground.
__unused
static uint16_t ColorBlend(Color_t fg, Color_t bg, uint16_t alpha) {
    uint16_t fg_ratio = alpha + 1;
    uint16_t bg_ratio = 256 - alpha;
    uint16_t r, g, b;

    r = fg.R * fg_ratio;
    g = fg.G * fg_ratio;
    b = fg.B * fg_ratio;

    r += bg.R * bg_ratio;
    g += bg.G * bg_ratio;
    b += bg.B * bg_ratio;

    r >>= 8;
    g >>= 8;
    b >>= 8;

    return RGBTo565(r, g, b);
}

// ==== Colors ====
#define clBlack     ((Color_t){0,   0,   0})
#define clRed       ((Color_t){255, 0,   0})
#define clGreen     ((Color_t){0,   255, 0})
#define clBlue      ((Color_t){0,   0,   255})
#define clYellow    ((Color_t){255, 255, 0})
#define clMagenta   ((Color_t){255, 0, 255})
#define clCyan      ((Color_t){0, 255, 255})
#define clWhite     ((Color_t){255, 255, 255})

#define clGrey      ((Color_t){126, 126, 126})
#define clLightGrey ((Color_t){180, 180, 180})
#define clDarkGrey  ((Color_t){54, 54, 54})

#define CL_DARK_V       27
#define clDarkRed       ((Color_t){CL_DARK_V, 0,         0})
#define clDarkGreen     ((Color_t){0,         CL_DARK_V, 0})
#define clDarkBlue      ((Color_t){0,         0,         CL_DARK_V})
#define clDarkYellow    ((Color_t){CL_DARK_V, CL_DARK_V, 0})
#define clDarkMagenta   ((Color_t){CL_DARK_V, 0,         CL_DARK_V})
#define clDarkCyan      ((Color_t){0,         CL_DARK_V, CL_DARK_V})
#define clDarkWhite     ((Color_t){CL_DARK_V, CL_DARK_V, CL_DARK_V})

#define clLightBlue ((Color_t){90, 90, 255})

static inline int32_t Abs32(int32_t w) {
    return (w < 0)? -w : w;
}
#if 1 // ============================== HSL ====================================
struct ColorHSL_t {
    union {
        uint32_t DWord32;
        struct {
            uint16_t H;     // 0...360
            uint8_t S, L;   // 0...100
        };
    };
    void ToRGB(uint8_t *PR, uint8_t *PG, uint8_t *PB) const {
        // Calc chroma: 0...255
        int32_t S1 = ((int32_t)S * 255) / 100;
        int32_t L1 = ((int32_t)L * 255) / 100;
        int32_t C = 255 - Abs32(L1 * 2 - 255);  // <=> 1 - |2*L - 1|
        C = (C * S1) / 255;                     // <=> (1 - |2*L - 1|) * S
        // Tmp values
        int32_t X = 60 - Abs32((H % 120) - 60); // 0...60
        X = (C * X) / 60;
        int32_t m = L1 - C / 2; // To add the same amount to each component, to match lightness
        // RGB in first glance
        if     (H < 60)  { *PR = C+m; *PG = X+m; *PB = m;   } // [0; 60)
        else if(H < 120) { *PR = X+m; *PG = C+m; *PB = m;   }
        else if(H < 180) { *PR = m;   *PG = C+m; *PB = X+m; }
        else if(H < 240) { *PR = m;   *PG = X+m; *PB = C+m; }
        else if(H < 300) { *PR = X+m; *PG = m;   *PB = C+m; }
        else             { *PR = C+m; *PG = m;   *PB = X+m; } // [300; 360]
    }

    void ToRGB(Color_t &AColor) { ToRGB(&AColor.R, &AColor.G, &AColor.B); }

    ColorHSL_t(uint16_t AH, uint8_t AS, uint8_t AL) : H(AH), S(AS), L(AL) {}
} __attribute__((packed));
#endif

#if 1 // ============================== HSV ====================================
struct ColorHSV_t {
    union {
        uint32_t DWord32;
        struct {
            uint16_t H;     // 0...360
            uint8_t S, V;   // 0...100
        };
    };
    void ToRGB(uint8_t *PR, uint8_t *PG, uint8_t *PB) const {
        // Calc chroma: 0...255
        int32_t C = ((int32_t)V * (int32_t)S * 255) / 10000;
        // Tmp values
        int32_t X = 60 - Abs32((H % 120) - 60); // 0...60
        X = (C * X) / 60;
        int32_t m = (((int32_t)V * 255) / 100) - C; // To add the same amount to each component, to match lightness
        // RGB
        if     (H < 60)  { *PR = C+m; *PG = X+m; *PB = m;   } // [0; 60)
        else if(H < 120) { *PR = X+m; *PG = C+m; *PB = m;   }
        else if(H < 180) { *PR = m;   *PG = C+m; *PB = X+m; }
        else if(H < 240) { *PR = m;   *PG = X+m; *PB = C+m; }
        else if(H < 300) { *PR = X+m; *PG = m;   *PB = C+m; }
        else             { *PR = C+m; *PG = m;   *PB = X+m; } // [300; 360]
    }

    void ToRGB(Color_t &AColor) { ToRGB(&AColor.R, &AColor.G, &AColor.B); }

    ColorHSV_t(uint16_t AH, uint8_t AS, uint8_t AV) : H(AH), S(AS), V(AV) {}
} __attribute__((packed));
#endif
