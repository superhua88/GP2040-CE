#ifndef BATTERY_METER_ADDON_H_
#define BATTERY_METER_ADDON_H_

// ===================================================================
// GP2040-CE 插件 · 电量灯条（WS2812B，NeoPico 驱动）
// 6 颗灯显示电量：满电全绿，逐颗变红；充电中呼吸；<12% 全红闪烁
// 电量由本插件 ADC 自采样（GP26 分压），无需官方 Battery 插件
// ===================================================================
#include "gpaddon.h"
#include "enums.pb.h"

#ifndef BATTERY_METER_ENABLED
#define BATTERY_METER_ENABLED 0
#endif

#ifndef METER_LED_PIN
#define METER_LED_PIN 15
#endif
#ifndef METER_LED_COUNT
#define METER_LED_COUNT 6
#endif

#define BatteryMeterAddonName "Battery Meter"

class BatteryMeterAddon : public GPAddon {
public:
    virtual bool available();
    virtual void setup();
    virtual void preprocess() {}
    virtual void process();
    virtual void postprocess(bool sent) {}
    virtual void reinit() {}
    virtual std::string name() { return BatteryMeterAddonName; }

private:
    uint8_t readBatteryPercent();
    uint32_t _frame[METER_LED_COUNT];
    uint8_t _breath;
    bool _breathDir;
    uint32_t _lastAnim;
};

#endif // BATTERY_METER_ADDON_H_
