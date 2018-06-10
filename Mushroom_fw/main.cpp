/*
 * main.cpp
 *
 *  Created on: 20 февр. 2014 г.
 *      Author: g.kruglov
 */

#include "kl_lib.h"
#include "MsgQ.h"
#include "shell.h"
//#include "ColorProfile.h"
#include "SimpleSensors.h"
#include "buttons.h"
#include "board.h"
//#include "kl_adc.h"
#include "IntelLedEffs.h"
#include "radio_lvl1.h"
#include "SaveToFlash.h"

#if 1 // ======================== Variables and defines ========================
// Forever
EvtMsgQ_t<EvtMsg_t, MAIN_EVT_Q_LEN> EvtQMain;
static const UartParams_t CmdUartParams(115200, CMD_UART_PARAMS);
CmdUart_t Uart{&CmdUartParams};
void OnCmd(Shell_t *PShell);
void ITask();

//Color_t Clr(0, 255, 0, 0);
ColorHSV_t hsv(99, 100, 100);
PinOutput_t PwrPin { PWR_EN_PIN };
TmrKL_t TmrSave {MS2ST(3600), evtIdTimeToSave, tktOneShot};
//TmrKL_t TmrAdc {MS2ST(450), evtIdEverySecond, tktPeriodic};
//Profile_t Profile;

//bool AdcFirstConv = true;

//void BtnHandler(BtnEvt_t BtnEvt);
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

    // Power pin
    PwrPin.Init();
    PwrPin.SetHi(); // PwrOff

    LedEffectsInit();

    if(Radio.Init() != retvOk) {
        for(int i=0; i<4; i++) {
            EffAllTogetherNow.SetupAndStart(clRed);
            chThdSleepMilliseconds(180);
            EffAllTogetherNow.SetupAndStart(clBlack);
            chThdSleepMilliseconds(180);
        }
    }

    // Load and check color
    Flash::Load((uint32_t*)&hsv, sizeof(ColorHSV_t));
    if(hsv.H > 360) hsv.H = 120;
    hsv.S = 100;
    hsv.V = 100;
    hsv.ToRGB().Print();

    EffOneByOne.SetupAndStart(hsv.ToRGB(), 360);

    SimpleSensors::Init();
    // Adc
//    PinSetupAnalog(BAT_MEAS_PIN);
//    Adc.Init();
//    Adc.EnableVRef();
//    TmrAdc.InitAndStart();
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
                EffAllTogetherNow.SetupAndStart(hsv.ToRGB());
                // Prepare to save
                TmrSave.StartOrRestart();
                break;

            case evtIdTimeToSave:
                Flash::Save((uint32_t*)&hsv, sizeof(ColorHSV_t));
                EffAllTogetherNow.SetupAndStart(clBlack);
                chThdSleepMilliseconds(153);
                EffAllTogetherNow.SetupAndStart(hsv.ToRGB());
                break;

            case evtIdRadioCmd: {
                Color_t Clr;
                Clr.DWord32 = Msg.Value;
                EffOneByOne.SetupAndStart(Clr, 360);
            }
            break;

            default: break;
        } // switch


//        Effects.AllTogetherNow(hsv);
//        hsv.H++;
//        if(hsv.H > 360) hsv.H = 0;
//        chThdSleepMilliseconds(90);


//        Effects.AllTogetherSmoothly(clRed, 360);
//        chThdSleepMilliseconds(2700);
//        Effects.AllTogetherSmoothly(clGreen, 360);
//        chThdSleepMilliseconds(2700);
//        Effects.AllTogetherSmoothly(clBlue, 360);
//        chThdSleepMilliseconds(2700);

#if ADC_REQUIRED
        if(Evt & EVT_SAMPLING) Adc.StartMeasurement();
        if(Evt & EVT_ADC_DONE) {
            if(AdcFirstConv) AdcFirstConv = false;
            else {
                uint32_t VBat_adc = Adc.GetResult(ADC_BAT_CHNL);
                uint32_t VRef_adc = Adc.GetResult(ADC_VREFINT_CHNL);
                __unused int32_t Vbat_mv = (2 * Adc.Adc2mV(VBat_adc, VRef_adc));   // Resistor divider
//                Uart.Printf("VBat_adc: %u; Vref_adc: %u; VBat_mv: %u\r", VBat_adc, VRef_adc, Vbat_mv);
//                if(Vbat_mv < 3600) SignalEvt(EVT_BATTERY_LOW);
            } // if not big diff
        } // evt
#endif
    } // while true
} // App_t::ITask()

//void BtnHandler(BtnEvt_t BtnEvt) {
//    if(BtnEvt == beShortPress) Uart.Printf("Btn Short\r");
//    if(BtnEvt == beLongPress)  Uart.Printf("Btn Long\r");
//    if(BtnEvt == beRelease)    Uart.Printf("Btn Release\r");
//}

void OnCmd(Shell_t *PShell) {
	Cmd_t *PCmd = &PShell->Cmd;
    __attribute__((unused)) int32_t dw32 = 0;  // May be unused in some configurations
//    Uart.Printf("%S\r", PCmd->Name);
    // Handle command
    if(PCmd->NameIs("Ping")) {
        PShell->Ack(retvOk);
    }

    else if(PCmd->NameIs("RGBW")) {
        Color_t Clr(0,0,0,0);
        if(PCmd->GetNext<uint8_t>(&Clr.R) != retvOk) return;
        if(PCmd->GetNext<uint8_t>(&Clr.G) != retvOk) return;
        if(PCmd->GetNext<uint8_t>(&Clr.B) != retvOk) return;
        if(PCmd->GetNext<uint8_t>(&Clr.W) != retvOk) return;
        EffAllTogetherNow.SetupAndStart(Clr);
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
