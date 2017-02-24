/*
 * evt_mask.h
 *
 *  Created on: Apr 12, 2013
 *      Author: g.kruglov
 */

#pragma once

// ==== Event masks ====
// Eternal
#define EVT_EVERY_SECOND        EVENT_MASK(1)
#define EVT_UART_NEW_CMD        EVENT_MASK(2)

// Periphery and may-be's
#define EVT_BUTTONS             EVENT_MASK(3)
#define EVT_IMPACT              EVENT_MASK(4)
#define EVT_BATTERY_LOW         EVENT_MASK(5)
#define EVT_LED_DONE            EVENT_MASK(7)
// Adc
#define EVT_SAMPLING            EVENT_MASK(9)
#define EVT_ADC_DONE            EVENT_MASK(10)
