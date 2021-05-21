#include "board.h"
#include "led.h"
#include "vibro.h"
#include "beeper.h"
#include "Sequences.h"
#include "kl_i2c.h"
#include "kl_lib.h"
#include "kl_buf.h"
#include "MsgQ.h"
#include "SimpleSensors.h"
#include "buttons.h"

#if 1 // ======================== Variables and defines ========================
// Forever
EvtMsgQ_t<EvtMsg_t, MAIN_EVT_Q_LEN> EvtQMain;
static const UartParams_t CmdUartParams(115200, CMD_UART_PARAMS);
CmdUart_t Uart{&CmdUartParams};
static void ITask();
static void OnCmd(Shell_t *PShell);

// ==== Timers ====
static TmrKL_t TmrEverySecond {TIME_MS2I(1000), evtIdEverySecond, tktPeriodic};
#endif

static inline void Set_A0_B0() {
    PinSetLo(GPIOA, 1);
    PinSetLo(GPIOA, 2);
    GPIOA->MODER &= ~((0b11UL << 2*1) | (0b11UL << 2*2)); // Inputs
    GPIOA->MODER |=  ((0b01UL << 2*1) | (0b01UL << 2*2)); // Outputs
}

static inline void Set_AZ_BZ() {
    GPIOA->MODER &= ~((0b11UL << 2*1) | (0b11UL << 2*2)); // Inputs
}

static inline void Set_A1() {
    PinSetHi(GPIOA, 1);
    PinSetupModeOut(GPIOA, 1);
}

static inline void Set_B0() {
    PinSetLo(GPIOA, 2);
    PinSetupModeOut(GPIOA, 2);
    DelayLoop(4);
}

static inline void Make_A_Input() {
    GPIOA->MODER &= ~(0b11UL << (2*1));
}

int main(void) {
    // ==== Init Vcore & clock system ====
    SetupVCore(vcore1V5);
    Clk.SetMSI4MHz();
    Clk.UpdateFreqValues();

    // === Init OS ===
    halInit();
    chSysInit();
    EvtQMain.Init();

    // ==== Init hardware ====
    Uart.Init();
    Printf("\r%S %S\r", APP_NAME, XSTRINGIFY(BUILD_TIME));
    Clk.PrintFreqs();

//    TmrEverySecond.StartOrRestart();

    PinSetupOut(GPIOA, 1, omPushPull, psMedium);
    PinSetupOut(GPIOA, 2, omPushPull, psMedium);

    uint32_t cnt = 0;
    while(true) {
        // Reset
        Set_A0_B0(); // Discharge them all

        Printf("%u\r", cnt);
        chThdSleepMilliseconds(27);

        // Measure
        cnt = 0;
        while(true) {
            if(++cnt >= 9000) break;
            Set_AZ_BZ();
            Set_A1(); // Charge Cx
            Set_AZ_BZ();
            Set_B0(); // Transfer charge from Cx to Cref
//            Make_A_Input();
            if(PinIsHi(GPIOA, 1)) break; // check if threshold reached
        }
    }

    // Main cycle
    ITask();
}

__noreturn
void ITask() {
    while(true) {
        EvtMsg_t Msg = EvtQMain.Fetch(TIME_INFINITE);
        switch(Msg.ID) {
            case evtIdEverySecond:
                break;

            case evtIdShellCmd:
                OnCmd((Shell_t*)Msg.Ptr);
                ((Shell_t*)Msg.Ptr)->SignalCmdProcessed();
                break;
            default: Printf("Unhandled Msg %u\r", Msg.ID); break;
        } // Switch
    } // while true
} // ITask()

#if 1 // ================= Command processing ====================
void OnCmd(Shell_t *PShell) {
	Cmd_t *PCmd = &PShell->Cmd;
    __attribute__((unused)) int32_t dw32 = 0;  // May be unused in some configurations
//    Uart.Printf("%S\r", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) {
        PShell->Ack(retvOk);
    }
    else if(PCmd->NameIs("Version")) PShell->Print("%S %S\r", APP_NAME, XSTRINGIFY(BUILD_TIME));

    else PShell->Ack(retvCmdUnknown);
}
#endif
