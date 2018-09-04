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
#include "Sequences.h"

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

    if(Radio.Init() == retvOk) {
        Leds.SeqAllTogetherStartOrRestart(lsqIdle);
    }
    else {
        Leds.SeqAllTogetherStartOrRestart(lsqFailure);
        chThdSleepMilliseconds(1008);
    }

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

            case evtIdRadioNoone:  Leds.SeqAllTogetherStartOrRestart (lsqIdle); break;
            case evtIdRadioButton: Leds.SeqAllTogetherStartOrContinue(lsqButton); break;
            case evtIdRadioLowPwr: Leds.SeqAllTogetherStartOrContinue(lsqLowPower); break;
            case evtIdRadioHiPwr:  Leds.SeqAllTogetherStartOrContinue(lsqHiPower); break;

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
        Color_t Clr;
        if(PCmd->GetNext<uint8_t>(&Clr.R) != retvOk) return;
        if(PCmd->GetNext<uint8_t>(&Clr.G) != retvOk) return;
        if(PCmd->GetNext<uint8_t>(&Clr.B) != retvOk) return;
        Leds.AllTogetherNow(Clr);
        PShell->Ack(retvOk);
    }

    else PShell->Ack(retvCmdUnknown);
}
