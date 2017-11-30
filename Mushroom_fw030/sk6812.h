/*
 * sk6812.h
 *
 *  Created on: 19-07-2017 �.
 *      Author: Kreyl
 */

#pragma once

/*
 * ========== SK2812 control module ==========
 * Only basic command "SetCurrentColors" is implemented, all other is up to
 * higher level software.
 * SPI input frequency should be 8 MHz (which results in 4MHz bitrate)
 */


#include "ch.h"
#include "hal.h"
#include "kl_lib.h"
#include "color.h"
#include "uart.h"

#define LED_CNT             150   // Number of LEDs

#define SEQ_LEN             4   // SPI bits per single bit of LED data
#define RST_W_CNT           4   // zero words before and after data to produce reset

// SPI16 Buffer (no tuning required)
#define DATA_BIT_CNT        (LED_CNT * 4 * 8 * SEQ_LEN) // Each led has 4 channels 8 bit each
#define DATA_W_CNT          ((DATA_BIT_CNT + 15) / 16)  // Data len in 16-bit words
#define TOTAL_W_CNT         (DATA_W_CNT + RST_W_CNT)

class LedSk_t {
private:
    Spi_t ISpi {LEDWS_SPI};
    uint16_t IBuf[TOTAL_W_CNT];
    uint16_t *PBuf;
    void AppendBitsMadeOfByte(uint8_t Byte);
public:
    void Init();
    bool AreOff() {
        for(uint8_t i=0; i<LED_CNT; i++) {
            if(ICurrentClr[i] != clRGBWBlack) return false;
        }
        return true;
    }
    // Inner use
    Color_t ICurrentClr[LED_CNT];
    void ISetCurrentColors();
    void ITmrHandlerI();
};

extern LedSk_t Leds;

#if 1 // ============================== Effects ================================
enum EffState_t {effIdle, effAllSmoothly};

class LedChunk_t {
private:
    int Head, Tail;
    uint8_t GetNext(int *PCurrent);
    int GetPrevN(int Current, int N);
public:
    int Start, End, NLeds;
    Color_t Color;
    LedChunk_t(int AStart, int AEnd) {
        Start = AStart;
        End = AEnd;
        NLeds = 1;
        Head = AStart;
        Tail = AEnd;
        Color = clBlack;
    }
    uint32_t ProcessAndGetDelay();
    void StartOver();
};

class Effects_t {
private:
    thread_t *PThd;
    EffState_t IState;
    uint32_t SmoothValue[LED_CNT];
    void IProcessChunkRandom();
public:
    void Init();
    // Effects
    void AllTogetherNow(Color_t Color);
    void AllTogetherNow(ColorHSV_t Color);
    void AllTogetherSmoothly(Color_t Color, uint32_t ASmoothValue);
//    void AllTogetherSmoothly(ColorHSV_t Color, uint32_t ASmoothValue);
    void ChunkRunningRandom(Color_t Color, uint32_t NLeds, uint32_t ASmoothValue);
    // Inner use
    uint32_t ICalcDelayN(uint32_t n);
    Color_t DesiredClr[LED_CNT];
    void ITask();
};

extern Effects_t Effects;
#endif