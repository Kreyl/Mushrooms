/*
 * SimpleSensors.h
 *
 *  Created on: 17 ���. 2015 �.
 *      Author: Kreyl
 */

#ifndef KL_LIB_SIMPLESENSORS_H_
#define KL_LIB_SIMPLESENSORS_H_

/*
 * Simple sensors are sensors with two logic states: Low and High.
 */

#include "hal.h"
#include <kl_lib_f100.h>
#include <PinSnsSettings.h>

#if SIMPLESENSORS_ENABLED
class SimpleSensors_t {
private:
    PinSnsState_t States[PIN_SNS_CNT];
public:
    void Init();
    void Shutdown() { for(uint32_t i=0; i<PIN_SNS_CNT; i++) PinSns[i].Off(); }
    // Inner use
    void ITask();
};

extern SimpleSensors_t PinSensors;
#endif

#endif /* KL_LIB_SIMPLESENSORS_H_ */
