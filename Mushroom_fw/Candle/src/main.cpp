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
#include "keys.h"

extern IWDG_t Iwdg;

int main(void) {
    // Check if Iwdg reset occured
    if(Iwdg.ResetOccured()) {
        Clk.UpdateFreqValues();
        // Init key pins
        PinSetupIn(GPIOB, 6, pudPullUp);
        PinSetupIn(GPIOB, 7, pudPullUp);
        PinSetupIn(GPIOA, 1, pudPullUp);
        Delay_ms(2);
        // Get back to sleep if nothing pressed
        if(PinIsSet(GPIOB, 6) and PinIsSet(GPIOB, 7) and PinIsSet(GPIOA, 1)) Iwdg.GoSleepFor(SLEEP_TIME_MS);
    }

    // ==== Init clock system ====
    Clk.SetupBusDividers(ahbDiv2, apbDiv1, apbDiv1);
    Clk.UpdateFreqValues();
    // ==== Init OS ====
    halInit();
    chSysInit();
    // ==== Init Hard & Soft ====
    JtagDisable();
    Uart.Init(115200);

    //srand(GetUniqID32());       // Init random generator using MCU uniq ID as seed
    LedWs.Init();
//    LedWs.SetCommonColorSmoothly(clGreen, csmSimultaneously);
    Radio.Init();
    Keys.Init();
    App.PThd = chThdSelf();
    Uart.Printf("\rCandle  AHB=%u; APB1=%u; APB2=%u\r\n", Clk.AHBFreqHz, Clk.APB1FreqHz, Clk.APB2FreqHz);
    App.Init();

    while(true) App.ITask();
}
