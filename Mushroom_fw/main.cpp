#include "board.h"
#include "led.h"
#include "Sequences.h"
#include "radio_lvl1.h"
#include "kl_lib.h"
#include "MsgQ.h"
#include "main.h"
#include "SimpleSensors.h"
#include "buttons.h"

#if 1 // ======================== Variables and defines ========================
// Forever
EvtMsgQ_t<EvtMsg_t, MAIN_EVT_Q_LEN> EvtQMain;
extern CmdUart_t Uart;
static void ITask();
static void OnCmd(Shell_t *PShell);

// ==== Periphery ====
LedRGB_t Led1 { LED1_R_PIN, LED1_G_PIN, LED1_B_PIN };
LedRGB_t Led2 { LED2_R_PIN, LED2_G_PIN, LED2_B_PIN };
LedRGB_t Led3 { LED3_R_PIN, LED3_G_PIN, LED3_B_PIN };
LedRGB_t Led4 { LED4_R_PIN, LED4_G_PIN, LED4_B_PIN };
LedRGB_t* Leds[LEDS_CNT] = {&Led1, &Led2, &Led3, &Led4};

// ==== Timers ====
static TmrKL_t TmrEverySecond {MS2ST(1000), evtIdEverySecond, tktPeriodic};
#endif

int main(void) {
    // ==== Init Vcore & clock system ====
    SetupVCore(vcore1V2);
    Clk.UpdateFreqValues();

    // === Init OS ===
    halInit();
    chSysInit();
    EvtQMain.Init();

    // ==== Init hardware ====
    Uart.Init(115200);
    Printf("\r%S %S\r", APP_NAME, XSTRINGIFY(BUILD_TIME));
    Clk.PrintFreqs();

//    SimpleSensors::Init();

    for(LedRGB_t* Led: Leds) {
        Led->Init();
        Led->SetColor({64, 128, 200});
    }

    // ==== Time and timers ====
    TmrEverySecond.StartOrRestart();

    // ==== Radio ====
//    if(
    Radio.Init();
    //== retvOk) EvtQMain.SendNowOrExit(EvtMsg_t(evtIdEverySecond)); // check mode now
//    else {
//        Led.StartOrRestart(lsqFailure);
//        chThdSleepMilliseconds(1008);
//    }

    // Main cycle
    ITask();
}

__noreturn
void ITask() {
    while(true) {
        EvtMsg_t Msg = EvtQMain.Fetch(TIME_INFINITE);
        switch(Msg.ID) {
            case evtIdEverySecond:
//                Printf("t\r");
                break;

            case evtIdButtons:
//                Printf("Btn\r");
                break;

            case evtIdShellCmd:
                OnCmd((Shell_t*)Msg.Ptr);
                ((Shell_t*)Msg.Ptr)->SignalCmdProcessed();
                break;
            default: Printf("Unhandled Msg %u\r", Msg.ID); break;
        } // Switch
    } // while true
} // ITask()

#if UART_RX_ENABLED // ================= Command processing ====================
void OnCmd(Shell_t *PShell) {
	Cmd_t *PCmd = &PShell->Cmd;
    __attribute__((unused)) int32_t dw32 = 0;  // May be unused in some configurations
    // Handle command
    if(PCmd->NameIs("Ping")) {
        PShell->Ack(retvOk);
    }
    else if(PCmd->NameIs("Version")) PShell->Printf("%S %S\r", APP_NAME, XSTRINGIFY(BUILD_TIME));

//    else if(PCmd->NameIs("GetID")) PShell->Reply("ID", ID);
//
//    else if(PCmd->NameIs("SetID")) {
//        if(PCmd->GetNext<int32_t>(&ID) != retvOk) { PShell->Ack(retvCmdError); return; }
//        uint8_t r = ISetID(ID);
//        RMsg_t msg;
//        msg.Cmd = R_MSG_SET_CHNL;
//        msg.Value = ID2RCHNL(ID);
//        Radio.RMsgQ.SendNowOrExit(msg);
//        PShell->Ack(r);
//    }

    else PShell->Ack(retvCmdUnknown);
}
#endif

#if 0 // =========================== ID management =============================
void ReadIDfromEE() {
    ID = EE::Read32(EE_ADDR_DEVICE_ID);  // Read device ID
    if(ID < ID_MIN or ID > ID_MAX) {
        Printf("\rUsing default ID\r");
        ID = ID_DEFAULT;
    }
}

uint8_t ISetID(int32_t NewID) {
    if(NewID < ID_MIN or NewID > ID_MAX) return retvFail;
    uint8_t rslt = EE::Write32(EE_ADDR_DEVICE_ID, NewID);
    if(rslt == retvOk) {
        ID = NewID;
        Printf("New ID: %u\r", ID);
        return retvOk;
    }
    else {
        Printf("EE error: %u\r", rslt);
        return retvFail;
    }
}
#endif
