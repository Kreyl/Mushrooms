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
#include "radio_lvl1.h"

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

ColorHSV_t hsv(99, 100, 100);
TmrKL_t TmrSave {MS2ST(3600), evtIdTimeToSave, tktOneShot};
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

    // Load and check color
    Flash::Load((uint32_t*)&hsv, sizeof(ColorHSV_t));
    if(hsv.H > 360) hsv.H = 0;
    hsv.S = 100;
    hsv.V = 100;

    // Leds
    Npx.Init();
    CommonEffectsInit();
    Leds.AllTogetherSmoothly(hsv.ToRGB(), 360);

    SimpleSensors::Init();

    Radio.Init(); // Init and enter PwrDown

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

            case evtIdButtons:
//                Printf("Btn %u\r", Msg.BtnEvtInfo.BtnID);
                if(Msg.BtnEvtInfo.BtnID == 0) {
                    if(hsv.H < 360) hsv.H++;
                    else hsv.H = 0;
                }
                else if(Msg.BtnEvtInfo.BtnID == 1) {
                    if(hsv.H > 0) hsv.H--;
                    else hsv.H = 360;
                }
//                Printf("HSV %u; ", hsv.H);
//                hsv.ToRGB().Print();
                Leds.AllTogetherNow(hsv.ToRGB());
                // Prepare to save
                TmrSave.StartOrRestart();
                break;

            case evtIdTimeToSave:
                Flash::Save((uint32_t*)&hsv, sizeof(ColorHSV_t));
                Leds.AllTogetherNow(clBlack);
                chThdSleepMilliseconds(153);
                Leds.AllTogetherNow(hsv.ToRGB());
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

//    else if(PCmd->NameIs("RGB")) {
//        if(PCmd->GetNext<uint8_t>(&Clr.R) != retvOk) return;
//        if(PCmd->GetNext<uint8_t>(&Clr.G) != retvOk) return;
//        if(PCmd->GetNext<uint8_t>(&Clr.B) != retvOk) return;
//        Leds.AllTogetherNow(Clr);
//        chThdSleepMilliseconds(999);
//        Leds.AllTogetherNow(clBlack);
//        Flash::Save((uint32_t*)&Clr.DWord32, sizeof(uint32_t));
//        PShell->Ack(retvOk);
//    }

    else if(PCmd->NameIs("HSV")) {
        ColorHSV_t IClrHsv(0,0,0);
        if(PCmd->GetNext<uint16_t>(&IClrHsv.H) != retvOk) { PShell->Ack(retvCmdError); return; }
        if(PCmd->GetNext<uint8_t>(&IClrHsv.S)  != retvOk) { PShell->Ack(retvCmdError); return; }
        if(PCmd->GetNext<uint8_t>(&IClrHsv.V)  != retvOk) { PShell->Ack(retvCmdError); return; }
        hsv = IClrHsv;
        Leds.AllTogetherNow(hsv.ToRGB());
        TmrSave.StartOrRestart();
        PShell->Ack(retvOk);
    }

    else PShell->Ack(retvCmdUnknown);
}
