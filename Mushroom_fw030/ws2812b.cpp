/*
 * ws2812b.cpp
 *
 *  Created on: 05 апр. 2014 г.
 *      Author: Kreyl
 */

#include "ws2812b.h"
#include "evt_mask.h"
#include "main.h"

#define LED_DMA_MODE    DMA_PRIORITY_HIGH \
                        | STM32_DMA_CR_MSIZE_HWORD \
                        | STM32_DMA_CR_PSIZE_HWORD \
                        | STM32_DMA_CR_MINC     /* Memory pointer increase */ \
                        | STM32_DMA_CR_DIR_M2P  /* Direction is memory to peripheral */ \
                        | STM32_DMA_CR_TCIE     /* Enable Transmission Complete IRQ */

// Tx timings: bit cnt
#define SEQ_1               0b11111000  // 0xF8
#define SEQ_0               0b11000000  // 0xC0

#define SEQ_00              0xC0C0
#define SEQ_01              0xC0F8
#define SEQ_10              0xF8C0
#define SEQ_11              0xF8F8


LedWs_t LedWs;

extern "C" {
// Wrapper for Tx Completed IRQ
void LedTxcIrq(void *p, uint32_t flags) {
    dmaStreamDisable(LEDWS_DMA);
//    Uart.PrintfI("Irq\r");
}
} // "C"

void LedWs_t::Init() {
    PinSetupAlterFunc(LEDWS_PIN);
    ISpi.Setup(boMSB, cpolIdleLow, cphaFirstEdge, sbFdiv2, bitn16);
    ISpi.Enable();
    ISpi.EnableTxDma();

    // Zero buffer
    for(uint32_t i=0; i<RST_W_CNT; i++) IBuf[i] = 0;

    // ==== DMA ====
    dmaStreamAllocate     (LEDWS_DMA, IRQ_PRIO_LOW, LedTxcIrq, NULL);
    dmaStreamSetPeripheral(LEDWS_DMA, &LEDWS_SPI->DR);
    dmaStreamSetMode      (LEDWS_DMA, LED_DMA_MODE);
}

void LedWs_t::AppendBitsMadeOfByte(uint8_t Byte) {
    uint8_t Bits;
    Bits = Byte & 0b11000000;
    if     (Bits == 0b00000000) *PBuf++ = SEQ_00;
    else if(Bits == 0b01000000) *PBuf++ = SEQ_01;
    else if(Bits == 0b10000000) *PBuf++ = SEQ_10;
    else if(Bits == 0b11000000) *PBuf++ = SEQ_11;

    Bits = Byte & 0b00110000;
    if     (Bits == 0b00000000) *PBuf++ = SEQ_00;
    else if(Bits == 0b00010000) *PBuf++ = SEQ_01;
    else if(Bits == 0b00100000) *PBuf++ = SEQ_10;
    else if(Bits == 0b00110000) *PBuf++ = SEQ_11;

    Bits = Byte & 0b00001100;
    if     (Bits == 0b00000000) *PBuf++ = SEQ_00;
    else if(Bits == 0b00000100) *PBuf++ = SEQ_01;
    else if(Bits == 0b00001000) *PBuf++ = SEQ_10;
    else if(Bits == 0b00001100) *PBuf++ = SEQ_11;

    Bits = Byte & 0b00000011;
    if     (Bits == 0b00000000) *PBuf++ = SEQ_00;
    else if(Bits == 0b00000001) *PBuf++ = SEQ_01;
    else if(Bits == 0b00000010) *PBuf++ = SEQ_10;
    else if(Bits == 0b00000011) *PBuf++ = SEQ_11;
}

void LedWs_t::ISetCurrentColors() {
    // Fill bit buffer
    PBuf = &IBuf[RST_W_CNT];
    for(uint32_t i=0; i<LED_CNT; i++) {
        AppendBitsMadeOfByte(ICurrentClr[i].G);
        AppendBitsMadeOfByte(ICurrentClr[i].R);
        AppendBitsMadeOfByte(ICurrentClr[i].B);
    }
//    AppendOnes();
    *PBuf = 0xFFFF;

    // Start transmission
    dmaStreamSetMemory0(LEDWS_DMA, IBuf);
    dmaStreamSetTransactionSize(LEDWS_DMA, TOTAL_W_CNT);
    dmaStreamSetMode(LEDWS_DMA, LED_DMA_MODE);
    dmaStreamEnable(LEDWS_DMA);
}
