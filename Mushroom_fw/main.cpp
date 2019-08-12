#include "board.h"
#include "led.h"
#include "Sequences.h"
#include "radio_lvl1.h"
#include "kl_lib.h"
#include "MsgQ.h"
#include "main.h"
#include "SimpleSensors.h"
#include "buttons.h"

PinOutputPWM_t Red({GPIOA, 2, TIM9, 1, invNotInverted, omPushPull, 255});

int main(void) {
    Clk.UpdateFreqValues();

    Red.Init();
    Red.SetFrequencyHz(10000);
    Red.Set(10);

    while(true) { }
}
