/*
 * Sequences.h
 *
 *  Created on: 09 ���. 2015 �.
 *      Author: Kreyl
 */

#pragma once

#include "ChunkTypes.h"

#if 1 // ============================ LED RGB ==================================
const LedRGBChunk_t lsqFailure[] = {
        {csSetup, 0, clRed},
        {csWait, 99},
        {csSetup, 0, clBlack},
        {csWait, 99},
        {csRepeat, 3},
        {csEnd}
};

const LedRGBChunk_t lsqIdle[] = {
        {csSetup, 720, {12, 12, 18}},
        {csEnd}
};

const LedRGBChunk_t lsqButton[] = {
        {csSetup, 360, {255, 0, 0}},
        {csSetup, 360, {153, 0, 0}},
        {csSetup, 180, {220, 0, 0}},
        {csSetup, 360, {180, 0, 0}},
        {csSetup, 300, {240, 0, 0}},
        {csSetup, 300, {153, 0, 0}},
        {csSetup, 360, {255, 0, 0}},
        {csSetup, 180, {220, 0, 0}},
        {csSetup, 153, {240, 0, 0}},
        {csSetup, 180, {255, 0, 0}},
        {csSetup, 120, {153, 0, 0}},
        {csSetup, 153, {240, 0, 0}},
        {csSetup, 360, {153, 0, 0}},
        {csSetup, 180, {220, 0, 0}},
        {csSetup, 153, {240, 0, 0}},
        {csSetup, 360, {255, 0, 0}},
        {csSetup, 360, {240, 0, 0}},
        {csSetup, 180, {153, 0, 0}},
        {csSetup, 120, {220, 0, 0}},
        {csGoto, 0}
};

const LedRGBChunk_t lsqLowPower[] = {
        {csSetup, 360, {90, 0, 0}},
        {csEnd}
};

const LedRGBChunk_t lsqHiPower[] = {
        {csSetup, 180, {255, 0, 0}},
        {csSetup, 180, {200, 0, 0}},
        {csSetup, 90,  {108, 0, 0}},
        {csSetup, 180, {220, 0, 0}},
        {csSetup, 153, {153, 0, 0}},
        {csSetup, 153, {90, 0, 0}},
        {csSetup, 180, {255, 0, 0}},
        {csSetup, 90,  {153, 0, 0}},
        {csSetup, 153, {220, 0, 0}},
        {csSetup, 90,  {90, 0, 0}},
        {csSetup, 63,  {153, 0, 0}},
        {csSetup, 153, {220, 0, 0}},
        {csSetup, 180, {153, 0, 0}},
        {csSetup, 90,  {255, 0, 0}},
        {csSetup, 153, {108, 0, 0}},
        {csSetup, 180, {200, 0, 0}},
        {csSetup, 180, {153, 0, 0}},
        {csSetup, 90,  {200, 0, 0}},
        {csSetup, 63,  {108, 0, 0}},
        {csGoto, 0}
};


// ==== Pill ====
//const LedRGBChunk_t lsqPillCure[] = {
//        {csSetup, 0, clGreen},
//        {csWait, 999},
//        {csSetup, 0, clBlack},
//        {csEnd},
//};
//const LedRGBChunk_t lsqPillPanacea[] = {
//        {csSetup, 0, clWhite},
//        {csWait, 999},
//        {csSetup, 0, clBlack},
//        {csEnd},
//};
//const LedRGBChunk_t lsqPillBad[] = {
//        {csSetup, 0, clRed},
//        {csWait, 999},
//        {csSetup, 0, clBlack},
//        {csEnd},
//};

#endif

#if 0 // =========================== LED Smooth ================================
#define LED_TOP_BRIGHTNESS  255

const LedSmoothChunk_t lsqFadeIn[] = {
        {csSetup, 630, LED_TOP_BRIGHTNESS},
        {csEnd}
};
const LedSmoothChunk_t lsqFadeOut[] = {
        {csSetup, 630, 0},
        {csEnd}
};
const LedSmoothChunk_t lsqEnterActive[] = {
        {csSetup, 0, LED_TOP_BRIGHTNESS},
        {csEnd}
};
const LedSmoothChunk_t lsqEnterIdle[] = {
        {csSetup, 360, 0},
        {csEnd}
};

#endif

#if 1 // ============================= Beeper ==================================
#define BEEP_VOLUME     2   // Maximum 10

#if 1 // ==== Notes ====
#define La_2    880

#define Do_3    1047
#define Do_D_3  1109
#define Re_3    1175
#define Re_D_3  1245
#define Mi_3    1319
#define Fa_3    1397
#define Fa_D_3  1480
#define Sol_3   1568
#define Sol_D_3 1661
#define La_3    1720
#define Si_B_3  1865
#define Si_3    1976

#define Do_4    2093
#define Do_D_4  2217
#define Re_4    2349
#define Re_D_4  2489
#define Mi_4    2637
#define Fa_4    2794
#define Fa_D_4  2960
#define Sol_4   3136
#define Sol_D_4 3332
#define La_4    3440
#define Si_B_4  3729
#define Si_4    3951

// Length
#define OneSixteenth    90
#define OneEighth       (OneSixteenth * 2)
#define OneFourth       (OneSixteenth * 4)
#define OneHalfth       (OneSixteenth * 8)
#define OneWhole        (OneSixteenth * 16)
#endif

// Type, BEEP_VOLUME, freq
const BeepChunk_t bsqOn[] = {
        {csSetup, 10, 7000},
        {csEnd}
};

const BeepChunk_t bsqButton[] = {
        {csSetup, 1, 1975},
        {csWait, 54},
        {csSetup, 0},
        {csEnd}
};
const BeepChunk_t bsqBeepBeep[] = {
        {csSetup, BEEP_VOLUME, 1975},
        {csWait, 54},
        {csSetup, 0},
        {csWait, 54},
        {csSetup, BEEP_VOLUME, 1975},
        {csWait, 54},
        {csSetup, 0},
        {csEnd}
};

#if 1 // ==== Extensions ====
// Pill
const BeepChunk_t bsqBeepPillOk[] = {
        {csSetup, BEEP_VOLUME, Si_3},
        {csWait, 180},
        {csSetup, BEEP_VOLUME, Re_D_4},
        {csWait, 180},
        {csSetup, BEEP_VOLUME, Fa_D_4},
        {csWait, 180},
        {csSetup, 0},
        {csEnd}
};

const BeepChunk_t bsqBeepPillBad[] = {
        {csSetup, BEEP_VOLUME, Fa_4},
        {csWait, 180},
        {csSetup, BEEP_VOLUME, Re_4},
        {csWait, 180},
        {csSetup, BEEP_VOLUME, Si_3},
        {csWait, 180},
        {csSetup, 0},
        {csEnd}
};
#endif // ext
#endif // beeper

#if 1 // ============================== Vibro ==================================
#define VIBRO_VOLUME    100  // 1 to 100

#define VIBRO_SHORT_MS          99
#define VIBRO_REPEAT_PERIOD     1008

const BaseChunk_t vsqBrr[] = {
        {csSetup, VIBRO_VOLUME},
        {csWait, VIBRO_SHORT_MS},
        {csSetup, 0},
        {csWait, VIBRO_REPEAT_PERIOD},
        {csEnd}
};

const BaseChunk_t vsqBrrBrr[] = {
        {csSetup, VIBRO_VOLUME},
        {csWait, VIBRO_SHORT_MS},
        {csSetup, 0},
        {csWait, 99},
        {csSetup, VIBRO_VOLUME},
        {csWait, VIBRO_SHORT_MS},
        {csSetup, 0},
        {csWait, VIBRO_REPEAT_PERIOD},
        {csEnd}
};

const BaseChunk_t vsqBrrBrrBrr[] = {
        {csSetup, VIBRO_VOLUME},
        {csWait, VIBRO_SHORT_MS},
        {csSetup, 0},
        {csWait, 99},
        {csSetup, VIBRO_VOLUME},
        {csWait, VIBRO_SHORT_MS},
        {csSetup, 0},
        {csWait, 99},
        {csSetup, VIBRO_VOLUME},
        {csWait, VIBRO_SHORT_MS},
        {csSetup, 0},
        {csWait, VIBRO_REPEAT_PERIOD},
        {csEnd}
};

// ==== Health ====
const BaseChunk_t vsqIll[] = {
        {csSetup, VIBRO_VOLUME},
        {csWait, 999},
        {csSetup, 0},
        {csWait, 3600},
        {csSetup, VIBRO_VOLUME},
        {csWait, VIBRO_SHORT_MS},
        {csGoto, 2}
};

const BaseChunk_t vsqDeath[] = {
        {csSetup, VIBRO_VOLUME},
        {csWait, 999},
        {csSetup, 0},
        {csWait, 4005},
        {csSetup, VIBRO_VOLUME},
        {csWait, VIBRO_SHORT_MS},
        {csGoto, 2}
};

// Cataclysm
const BaseChunk_t vsqCataclysm[] = {
        {csSetup, VIBRO_VOLUME},
        {csWait, 999},
        {csSetup, 0},
        {csWait, 999},
        {csSetup, VIBRO_VOLUME},
        {csWait, VIBRO_SHORT_MS},
        {csSetup, 0},
        {csWait, 99},
        {csSetup, VIBRO_VOLUME},
        {csWait, VIBRO_SHORT_MS},
        {csGoto, 2}
};

/*
const BaseChunk_t vsqError[] = {
        {csSetup, VIBRO_VOLUME},
        {csWait, 999},
        {csSetup, 0},
        {csEnd}
};

const BaseChunk_t vsqSingle[] = {
        {csSetup, VIBRO_VOLUME},
        {csWait, VIBRO_SHORT_MS},
        {csSetup, 0},
        {csWait, 1800},
        {csGoto, 0}
};
const BaseChunk_t vsqPair[] = {
        {csSetup, VIBRO_VOLUME},
        {csWait, VIBRO_SHORT_MS},
        {csSetup, 0},
        {csWait, 99},
        {csSetup, VIBRO_VOLUME},
        {csWait, VIBRO_SHORT_MS},
        {csSetup, 0},
        {csWait, 1350},
        {csGoto, 0}
};
const BaseChunk_t vsqMany[] = {
        {csSetup, VIBRO_VOLUME},
        {csWait, VIBRO_SHORT_MS},
        {csSetup, 0},
        {csWait, 99},
        {csSetup, VIBRO_VOLUME},
        {csWait, VIBRO_SHORT_MS},
        {csSetup, 0},
        {csWait, 99},
        {csSetup, VIBRO_VOLUME},
        {csWait, VIBRO_SHORT_MS},
        {csSetup, 0},
        {csWait, 1008},
        {csGoto, 0}
};
*/
#endif