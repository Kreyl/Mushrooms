/*
 * main.cpp
 *
 *  Created on: 20 февр. 2014 г.
 *      Author: g.kruglov
 */

#include "kl_lib.h"
#include "MsgQ.h"
#include "shell.h"
#include "SimpleSensors.h"
#include "buttons.h"
#include "board.h"
#include "IntelLedEffs.h"
#include "SaveToFlash.h"
#include "main.h"

#if 1 // ======================== Variables and defines ========================
// Forever
EvtMsgQ_t<EvtMsg_t, MAIN_EVT_Q_LEN> EvtQMain;
static const UartParams_t CmdUartParams(115200, CMD_UART_PARAMS);
CmdUart_t Uart{&CmdUartParams};
void OnCmd(Shell_t *PShell);
void ITask();

static const NeopixelParams_t LedParams(
        NPX_SPI, NPX_GPIO, NPX_PIN, NPX_AF, NPX_DMA, NPX_DMA_MODE(NPX_DMA_CHNL),
        PWR_EN_PIN);
Neopixels_t Npx(&LedParams);
Effects_t Leds(&Npx);

Color_t Clr(207, 207, 255);

TmrKL_t TmrEverySecond {MS2ST(999), evtIdEverySecond, tktPeriodic};
#endif

int main(void) {
    // ==== Init Clock system ====
    Clk.UpdateFreqValues();

    // === Init OS ===
    halInit();
    chSysInit();

    // ==== Init hardware ====
    EvtQMain.Init();
    Uart.InitAndStartRx();
    Printf("\r%S %S\r", APP_NAME, XSTRINGIFY(BUILD_TIME));
    Clk.PrintFreqs();

    // Leds
    Npx.Init();
    CommonEffectsInit();

    TmrEverySecond.StartOrRestart();

    // Main cycle
    ITask();
}

__noreturn
void ITask() {
    while(true) {
        EvtMsg_t Msg = EvtQMain.Fetch(TIME_INFINITE);
        switch(Msg.ID) {
            case evtIdShellCmd:
                OnCmd((Shell_t*)Msg.Ptr);
                ((Shell_t*)Msg.Ptr)->SignalCmdProcessed();
                break;

            case evtIdEverySecond:
                break;

            default: break;
        } // switch
    } // while true
} // App_t::ITask()

void OnCmd(Shell_t *PShell) {
	Cmd_t *PCmd = &PShell->Cmd;
    if(PCmd->NameIs("Ping")) {
        PShell->Ack(retvOk);
    }

    else if(PCmd->NameIs("RGB")) {
        if(PCmd->GetNext<uint8_t>(&Clr.R) != retvOk) return;
        if(PCmd->GetNext<uint8_t>(&Clr.G) != retvOk) return;
        if(PCmd->GetNext<uint8_t>(&Clr.B) != retvOk) return;
        Leds.AllTogetherNow(Clr);
        chThdSleepMilliseconds(999);
        Leds.AllTogetherNow(clBlack);
        Flash::Save((uint32_t*)&Clr.DWord32, sizeof(uint32_t));
        PShell->Ack(retvOk);
    }

//    else if(PCmd->NameIs("HSV")) {
//        ColorHSV_t ClrHsv(0,0,0);
//        if(PCmd->GetNextUint16(&ClrHsv.H) != OK) return;
//        if(PCmd->GetNextByte(&ClrHsv.S)   != OK) return;
//        if(PCmd->GetNextByte(&ClrHsv.V)   != OK) return;
//        Color_t Clr;
//        ClrHsv.ToRGB(Clr);
////        Clr.Print();
////        Uart.Printf("{%u; %u; %u}\r", R, G, B);
////        Led.SetColor(Clr, 100);
//        PShell->Ack(OK);
//    }

    else PShell->Ack(retvCmdUnknown);
}
