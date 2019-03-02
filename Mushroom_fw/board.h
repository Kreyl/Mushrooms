#pragma once

#include <inttypes.h>

// ==== General ====
#define BOARD_NAME          "Mushroom5"
#define APP_NAME            "V5"

// ==== High-level peripery control ====
#define BUTTONS_ENABLED     FALSE

#define SIMPLESENSORS_ENABLED   BUTTONS_ENABLED

// MCU type as defined in the ST header.
#define STM32L151xB

// Freq of external crystal if any. Leave it here even if not used.
#define CRYSTAL_FREQ_HZ     12000000

#define SYS_TIM_CLK         (Clk.APB1FreqHz)
#define I2C_USE_SEMAPHORE   FALSE
#define ADC_REQUIRED        FALSE

#if 1 // ========================== GPIO =======================================
// PortMinTim_t: GPIO, Pin, Tim, TimChnl, invInverted, omPushPull, TopValue
// UART
#define UART_GPIO       GPIOA
#define UART_TX_PIN     9
#define UART_RX_PIN     10

// LEDs
#define LED1_R_PIN      { GPIOA, 1, TIM2, 2, invNotInverted, omPushPull, 255 }
#define LED1_G_PIN      { GPIOA, 2, TIM9, 1, invNotInverted, omPushPull, 255 }
#define LED1_B_PIN      { GPIOA, 3, TIM9, 2, invNotInverted, omPushPull, 255 }

#define LED2_R_PIN      { GPIOA, 5, TIM2, 1, invNotInverted, omPushPull, 255 }
#define LED2_G_PIN      { GPIOA, 6, TIM3, 1, invNotInverted, omPushPull, 255 }
#define LED2_B_PIN      { GPIOA, 7, TIM3, 2, invNotInverted, omPushPull, 255 }

#define LED3_R_PIN      { GPIOB, 0, TIM3, 3, invNotInverted, omPushPull, 255 }
#define LED3_G_PIN      { GPIOB, 1, TIM3, 4, invNotInverted, omPushPull, 255 }
#define LED3_B_PIN      { GPIOB, 6, TIM4, 1, invNotInverted, omPushPull, 255 }

#define LED4_R_PIN      { GPIOB, 7, TIM4, 2, invNotInverted, omPushPull, 255 }
#define LED4_G_PIN      { GPIOB, 8, TIM4, 3, invNotInverted, omPushPull, 255 }
#define LED4_B_PIN      { GPIOB, 9, TIM4, 4, invNotInverted, omPushPull, 255 }

#define LEDS_CNT        4

// Button
#define BTN_PIN         GPIOA, 0, pudPullDown

// Radio: SPI, PGpio, Sck, Miso, Mosi, Cs, Gdo0
#define CC_Setup0       SPI2, GPIOB, 13,14,15, 5, 2

#endif // GPIO

#if 1 // ========================== USART ======================================
#define CMD_UART        USART1
#define UART_TXBUF_SZ   256
#endif

#if ADC_REQUIRED // ======================= Inner ADC ==========================
// Clock divider: clock is generated from the APB2
#define ADC_CLK_DIVIDER     adcDiv2

// ADC channels
//#define BAT_CHNL          1

#define ADC_VREFINT_CHNL    17  // All 4xx, F072 and L151 devices. Do not change.
#define ADC_CHANNELS        { ADC_VREFINT_CHNL }
#define ADC_CHANNEL_CNT     1   // Do not use countof(AdcChannels) as preprocessor does not know what is countof => cannot check
#define ADC_SAMPLE_TIME     ast96Cycles
#define ADC_SAMPLE_CNT      8   // How many times to measure every channel

#define ADC_SEQ_LEN         (ADC_SAMPLE_CNT * ADC_CHANNEL_CNT)

#endif

#if 1 // =========================== DMA =======================================
#define STM32_DMA_REQUIRED  TRUE
// ==== Uart ====
#define UART_DMA_TX     STM32_DMA1_STREAM4
#define UART_DMA_RX     STM32_DMA1_STREAM5
#define UART_DMA_CHNL   0   // Dummy

#if ADC_REQUIRED
#define ADC_DMA         STM32_DMA1_STREAM1
#define ADC_DMA_MODE    STM32_DMA_CR_CHSEL(0) |   /* dummy */ \
                        DMA_PRIORITY_LOW | \
                        STM32_DMA_CR_MSIZE_HWORD | \
                        STM32_DMA_CR_PSIZE_HWORD | \
                        STM32_DMA_CR_MINC |       /* Memory pointer increase */ \
                        STM32_DMA_CR_DIR_P2M |    /* Direction is peripheral to memory */ \
                        STM32_DMA_CR_TCIE         /* Enable Transmission Complete IRQ */
#endif // ADC

#endif // DMA
