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

#include "PinSnsSettings.h"
#include "SimpleSensors.h"

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
    Uart.Printf("\rMushroom  AHB=%u; APB1=%u; APB2=%u\r\n", Clk.AHBFreqHz, Clk.APB1FreqHz, Clk.APB2FreqHz);

    LedWs.Init();
    LedWs.SetCommonColorSmoothly(ColorTable[32], csmSimultaneously);
//    Radio.Init();
    App.PThd = chThdSelf();
    App.Init();

    PinSensors.Init();

    while(true) App.ITask();
}
