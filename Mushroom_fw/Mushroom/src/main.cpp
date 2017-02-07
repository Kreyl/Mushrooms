/*
 * File:   main.cpp
 * Author: Kreyl
 * Project: Armlet2South
 *
 * Created on Feb 05, 2013, 20:27
 */

#include "led.h"
#include "kl_lib_f100.h"
#include "ch.h"
#include "hal.h"
#include "adc_f100.h"
#include "cmd_uart_f10x.h"
#include "ws2812b.h"
#include "application.h"
#include "radio_lvl1.h"

#define DELAY_MS    6300

int main(void) {
    // ==== Init clock system ====
    Clk.SetupBusDividers(ahbDiv2, apbDiv1, apbDiv1);
    Clk.UpdateFreqValues();
    // ==== Init OS ====
    halInit();
    chSysInit();
    // ==== Init Hard & Soft ====
    JtagDisable();
    Uart.Init(115200);

    LedWs.Init();
    Radio.Init();
    App.PThd = chThdSelf();
    App.Init();

    Uart.Printf("\rMushroom  AHB=%u; APB1=%u; APB2=%u\r\n", Clk.AHBFreqHz, Clk.APB1FreqHz, Clk.APB2FreqHz);
    while(true) {
        //App.ITask();
        LedWs.SetCommonColorSmoothly(clWhite, csmSimultaneously);
        chThdSleepMilliseconds(DELAY_MS);
        LedWs.SetCommonColorSmoothly(clRed, csmSimultaneously);
        chThdSleepMilliseconds(DELAY_MS);
        LedWs.SetCommonColorSmoothly(clGreen, csmSimultaneously);
        chThdSleepMilliseconds(DELAY_MS);
        LedWs.SetCommonColorSmoothly(clBlue, csmSimultaneously);
        chThdSleepMilliseconds(DELAY_MS);
        LedWs.SetCommonColorSmoothly(clYellow, csmSimultaneously);
        chThdSleepMilliseconds(DELAY_MS);
        LedWs.SetCommonColorSmoothly(clViolet, csmSimultaneously);
        chThdSleepMilliseconds(DELAY_MS);
        LedWs.SetCommonColorSmoothly(clCyan, csmSimultaneously);
        chThdSleepMilliseconds(DELAY_MS);
    }
}
