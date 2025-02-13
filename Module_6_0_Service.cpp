#include "MicroBitConfig.h"
#include "Module_6_0_Service.h"
#include "CryptTool.h"

static uint8_t _ctxValue = 0x25; // CTXValue for Encryption
static uint8_t _addressArray[5]     = { 0xC1, 0xC2, 0xC3, 0xC4, 0xC5 };
static uint8_t _telegram_Connect[8] = { 0x6D, 0x7B, 0xA7, 0x80, 0x80, 0x80, 0x80, 0x92, };


/**
 * Constructor.
 * Create a representation of the Module_6_0_Service
 * @param _BLEAdvManager The instance of a BLEAdvManager that we're running on.
 * @param moduleNo Number of the MK6 Module.
 */
Module_6_0_Service::Module_6_0_Service(BLEAdvManager &_BLEAdvManager, uint8_t moduleNo) : 
    m_bleAdvManager(_BLEAdvManager), 
    m_moduleNo(moduleNo) 
{

    m_bleAdvManager_handle = m_bleAdvManager.register_client(this);

    m_telegram_Data[0] = 0x61 + m_moduleNo;
    m_telegram_Data[9] = 0x9E - m_moduleNo;

    reset();
}


/**
 *  reset internal data to defaults
 */
void Module_6_0_Service::reset() 
{
    m_zeroHysteresisBounds_abs_pct = ZERO_HYSTERESIS_BOUNDS_DEFAULT;

    for (uint8_t index = 0; index < CHANNEL_COUNT; index++)
    {
        m_channelOffsets_abs_pct[index] = CHANNEL_OFFSET_DEFAULT;
        m_channelMaximums_abs_pct[index] = CHANNEL_MAXIMUM_DEFAULT;
        m_channelValues_byte[index] = CHANNEL_ZERO_VALUE;
        m_channelReverse[index] = CHANNEL_REVERSE_DEFAULT;
    }
}


/**
 *  set _telegram_Connect to advertiser
 */
void Module_6_0_Service::connect() 
{
    if (m_bleAdvManager_handle != UNSET_HANDLE) 
    {
        // MICROBIT_DEBUG_DMESG("Module_6_0_Service::connect");

        get_rf_payload(_addressArray, 5, _telegram_Connect, 8, _ctxValue, m_pPayload);

        m_bleAdvManager.advertise(m_bleAdvManager_handle, m_pPayload);
    }
}


/**
 *  stop advertising
 */
void Module_6_0_Service::stopAdvertising() 
{
    m_bleAdvManager.advertise_stop(m_bleAdvManager_handle);
}


/**
 *  set data to advertiser
 */
void Module_6_0_Service::setData() 
{
    if (m_bleAdvManager_handle != UNSET_HANDLE) 
    {
        memcpy(&m_telegram_Data[3], m_channelValues_byte, sizeof(uint8_t) * CHANNEL_COUNT);

        get_rf_payload(_addressArray, 5, m_telegram_Data, 10, _ctxValue, m_pPayload);

        m_bleAdvManager.advertise(m_bleAdvManager_handle, m_pPayload);
    }
}


/**
 *  set value (in percent) of channel
 * @param channelNo [0..5]
 * @param value_pct [0..100], eg: "80"
 */
void Module_6_0_Service::setChannelValue(uint8_t channelNo, float value_pct) 
{
    // MICROBIT_DEBUG_DMESG("Module_6_0_Service::setChannel");

    // check 
    if (channelNo >= CHANNEL_COUNT)
    {
        return;
    }

    if (value_pct >= -m_zeroHysteresisBounds_abs_pct && // is value_pct inside m_zeroHysteresisBounds_abs_pct?
        value_pct <= m_zeroHysteresisBounds_abs_pct) 
    {
        m_channelValues_byte[channelNo] = CHANNEL_ZERO_VALUE;
    }
    else if (value_pct < 0) 
    {
        float result_abs_pct = fmin(-value_pct + m_channelOffsets_abs_pct[channelNo], m_channelMaximums_abs_pct[channelNo]);
        float result_abs = fmin(0x80, result_abs_pct * 0x80 / 100);

        m_channelValues_byte[channelNo] = (uint8_t)fmax(0x00, 0x80 - result_abs);
    }
    else 
    {
        float result_abs_pct = fmin(value_pct + m_channelOffsets_abs_pct[channelNo], m_channelMaximums_abs_pct[channelNo]);
        float result_abs = fmin(0x80, result_abs_pct * 0x80 / 100);

        m_channelValues_byte[channelNo] = (uint8_t)fmin(0xFF, 0x80 + result_abs);
    }
}


/**
 *  get channel value in percent
 * @param channelNo [0..3]
 */
float Module_6_0_Service::getChannelValue(uint8_t channelNo) 
{
     // check 
    if (channelNo >= CHANNEL_COUNT)
    {
        return 0.0;
    }

    uint8_t originValue_byte = m_channelValues_byte[channelNo];

    if (originValue_byte == CHANNEL_ZERO_VALUE)
    {
        return 0.0;
    }
    else if (originValue_byte > CHANNEL_ZERO_VALUE)
    {
        return ((float)originValue_byte - CHANNEL_ZERO_VALUE) * 100.0 / 0x80;
    }
    else
    {
        return -((float)originValue_byte + CHANNEL_ZERO_VALUE) * 100.0 / 0x80;
    }
}


/**
 *  set offset (in percent) of channel
 * @param channelNo [0..5]
 * @param offset_pct [0..100], eg: "80"
 */
void Module_6_0_Service::setChannelOffset(uint8_t channelNo, float offset_pct) 
{
    // check 
    if (channelNo >= CHANNEL_COUNT)
    {
        return;
    }

    float offset_abs_pct = fabsf(offset_pct);

    if (m_channelOffsets_abs_pct[channelNo] != offset_abs_pct)
    {
        m_channelOffsets_abs_pct[channelNo] = offset_abs_pct;

        float value_pct = getChannelValue(channelNo);   // get current value
        float value_abs_pct = fabsf(value_pct);

        if (value_abs_pct < offset_abs_pct)             // new offset is greater than old one
        {
            setChannelValue(channelNo, value_pct);      // set value
        }
    }
}


/**
 *  set maximum value (in percent) of channel
 * @param channelNo [0..5]
 * @param maximum_pct [0..100], eg: "80"
 */
void Module_6_0_Service::setChannelMax(uint8_t channelNo, float maximum_pct) 
{
    // check 
    if (channelNo >= CHANNEL_COUNT)
    {
        return;
    }

    float maximum_abs_pct = fabsf(maximum_pct);

    if (m_channelMaximums_abs_pct[channelNo] != maximum_abs_pct)
    {
        m_channelMaximums_abs_pct[channelNo] = maximum_abs_pct;

        float value_pct = getChannelValue(channelNo);   // get current value
        float value_abs_pct = fabsf(value_pct);

        if (maximum_abs_pct < value_abs_pct)            // new maximum ist smaller than old one
        {
            setChannelValue(channelNo, value_pct);      // set value
        }
    }
}


/**
 *  set channel reverse
 * @param channelNo [0..3]
 * @param reverse [true, false], eg: "false"
 */
void Module_6_0_Service::setChannelReverse(uint8_t channelNo, bool reverse) 
{
     // check 
    if (channelNo >= CHANNEL_COUNT)
    {
        return;
    }

    if (m_channelReverse[channelNo] != reverse)
    {
        m_channelReverse[channelNo] = reverse;

        float value_pct = getChannelValue(channelNo);   // get current value
        setChannelValue(channelNo, -value_pct);         // set reverted value
    }
}


/**
 *  set bound value (in percent) zero hysteresis
 * @param zeroHysteresisBounds_pct default: 1.0
 */
void Module_6_0_Service::setZeroHysteresis(float zeroHysteresisBounds_pct) 
{
    m_zeroHysteresisBounds_abs_pct = fabsf(zeroHysteresisBounds_pct);
}


uint8_t Module_6_0_Service::getVersion() 
{
    return this->m_bleAdvManager_handle;
}
