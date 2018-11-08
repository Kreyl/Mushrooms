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
#include "radio_lvl1.h"
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

Color_t Clr;
ColorHSV_t ClrHsv;
TmrKL_t TmrSave {MS2ST(3600), evtIdTimeToSave, tktOneShot};

TmrKL_t TmrEverySecond {MS2ST(999), evtIdEverySecond, tktPeriodic};
static uint32_t AppearTimeout = 0;
static uint32_t TableCheckTimeout = CHECK_PERIOD_S;
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

    // Load and check color
    Flash::Load((uint32_t*)&Clr.DWord32, sizeof(uint32_t));
    if(Clr.R > 255) Clr.R = 255;
    if(Clr.G > 255) Clr.G = 255;
    if(Clr.B > 255) Clr.B = 255;
    if(Clr.Brt > 100) Clr.Brt = 100;
    Clr.Print();
    ClrHsv.FromRGB(Clr);

    if(Radio.Init() == retvOk) {
        Leds.AllTogetherNow(Clr);
        chThdSleepMilliseconds(999);
        Leds.AllTogetherNow(clBlack);
    }
    else {
        for(int i=0; i<4; i++) {
            Leds.AllTogetherNow(clRed);
            chThdSleepMilliseconds(180);
            Leds.AllTogetherNow(clBlack);
            chThdSleepMilliseconds(180);
        }
    }

//    EffOneByOne.SetupAndStart(hsv.ToRGB(), 360);

    TmrEverySecond.StartOrRestart();

    SimpleSensors::Init();
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
//                Printf("Second\r");
                if(AppearTimeout > 0) {
                    AppearTimeout--;
                    if(AppearTimeout == 0) Leds.OneByOne(clBlack, 720);
                }

                if(TableCheckTimeout > 0) {
                    TableCheckTimeout--;
                    if(TableCheckTimeout == 0) {
                        TableCheckTimeout = CHECK_PERIOD_S;
                        // Check table
//                        Printf("TblCnt: %u\r", Radio.RxTable.GetCount());
                        if(Radio.RxTable.GetCount() > 0) {
                            AppearTimeout = APPEAR_DURATION;
                            Leds.OneByOne(Clr, 720);
                            Radio.RxTable.Clear();
                        }
                    }
                }
                break;

            case evtIdButtons:
                AppearTimeout = APPEAR_DURATION;
//                Printf("Btn %u\r", Msg.BtnEvtInfo.BtnID);
                if(Msg.BtnEvtInfo.BtnID == 0) {
                    if(ClrHsv.H < 360) ClrHsv.H++;
                    else ClrHsv.H = 0;
                }
                else if(Msg.BtnEvtInfo.BtnID == 1) {
                    if(ClrHsv.H > 0) ClrHsv.H--;
                    else ClrHsv.H = 360;
                }
//                Printf("HSV %u; ", hsv.H);
//                hsv.ToRGB().Print();
                Leds.AllTogetherNow(ClrHsv.ToRGB());
                // Prepare to save
                TmrSave.StartOrRestart();

                break;

            case evtIdTimeToSave:
                Flash::Save((uint32_t*)&ClrHsv, sizeof(ColorHSV_t));
                Leds.AllTogetherNow(clBlack);
                chThdSleepMilliseconds(153);
                Leds.AllTogetherNow(ClrHsv.ToRGB());
                break;

            case evtIdRadioCmd: {
                Color_t Clr;
                Clr.DWord32 = Msg.Value;
                Leds.OneByOne(Clr, 360);
            }
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
