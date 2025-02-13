#include "pxt.h"
#include <stdio.h>
#include <ctype.h>
#include "MicroBitConfig.h"
#include "Module_6_0_Service.h"
#include "BLEAdvManager.h"

using namespace pxt;

/**
 * Support for M0uld K1ng Module 6.0.
 */
namespace mk_module_6_0 
{
    Module_6_0_Service* _pService[3] = 
    {
        NULL,
        NULL,
        NULL
    };


    //%
    Module_6_0_Service* getService(uint8_t moduleNo) 
    {
        Module_6_0_Service *pService = _pService[moduleNo];

        if (NULL == pService)
        {
            BLEAdvManager *p_BLEAdvManager = BLEAdvManager::getInstance();

            pService = new Module_6_0_Service(*p_BLEAdvManager, moduleNo);
            _pService[moduleNo] = pService;
        }

        return pService;
    }


    //%
    void init(uint8_t moduleNo) 
    {
        Module_6_0_Service *pService = getService(moduleNo);

        pService->connect();
    }


    //% 
    void setChannelValue(uint8_t moduleNo, uint8_t channel, float value_pct) 
    {
        Module_6_0_Service *pService = getService(moduleNo);

        pService->setChannelValue(channel, value_pct);
    }   


    //% 
    void setChannelOffset(uint8_t moduleNo, uint8_t channel, float offset_pct) 
    {
        Module_6_0_Service *pService = getService(moduleNo);

        pService->setChannelOffset(channel, offset_pct);
    }   


    //% 
    void setChannelMax(uint8_t moduleNo, uint8_t channel, float maximum_pct) 
    {
        Module_6_0_Service *pService = getService(moduleNo);

        pService->setChannelMax(channel, maximum_pct);
    }   


    //% 
    void setChannelReverse(uint8_t moduleNo, uint8_t channel, bool reverse) 
    {
        Module_6_0_Service *pService = getService(moduleNo);

        pService->setChannelReverse(moduleNo * 3 + channel, reverse);
    }   


    //% 
    void setZeroHysteresis(uint8_t moduleNo, float zeroHysteresis_pct) 
    {
        Module_6_0_Service *pService = getService(moduleNo);

        pService->setZeroHysteresis(zeroHysteresis_pct);
    }   


    //% 
    void reset(uint8_t moduleNo) 
    {
        Module_6_0_Service *pService = getService(moduleNo);

        pService->reset();
    }   


    //% 
    void setData(uint8_t moduleNo) 
    {
        Module_6_0_Service *pService = getService(moduleNo);

        pService->setData();
    }   


    //% 
    void stopAdvertising(uint8_t moduleNo) 
    {
        Module_6_0_Service *pService = getService(moduleNo);

        if (NULL != pService) 
        {
            pService->stopAdvertising();
        }        
    }   


    //% 
    uint8_t getVersion(uint8_t moduleNo) 
    {
        Module_6_0_Service *pService = getService(moduleNo);

        return pService->getVersion();
    }   
}