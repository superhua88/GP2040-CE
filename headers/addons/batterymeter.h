#ifndef BATTERY_METER_ADDON_H_
#define BATTERY_METER_ADDON_H_

// ===================================================================
// GP2040-CE 插件 · 电量灯条（WS2812B 幻彩 LED）
// 功能：6 颗灯显示电量，满电全绿，随电量递减逐颗变红/熄灭
//       充电时呼吸动画（绿灯流动），插 USB 时自动切换显示
// 硬件：WS2812B 灯条 x6（或 6 颗独立灯珠），数据线接 Pico GP15
//       灯条 5V 接 VBUS（插线时）/VSYS（电池时均可，3.9V 也能点亮 WS2812）
// 依赖：GP2040-CE 自带的 PicoLed 库（PicoRGBAddon 同款）+ Battery 插件
// ===================================================================
#include "gpaddon.h"
#include "enums.pb.h"

#ifndef BATTERY_METER_ENABLED
#define BATTERY_METER_ENABLED 0
#endif

// ---- 灯条配置 ----
#ifndef METER_LED_PIN
#define METER_LED_PIN 15        // WS2812 数据脚（避开 nRF24 的 GP2-6 和电源管理 GP7）
#endif
#ifndef METER_LED_COUNT
#define METER_LED_COUNT 6       // 六颗灯
#endif

// ---- 颜色（GRB 顺序，WS2812 标准）----
#define C_GREEN  0, 255, 0
#define C_RED    255, 0, 0
#define C_ORANGE 255, 80, 0
#define C_OFF    0, 0, 0

// ---- 阈值 ----
// 6 灯梯度：<10% 全红闪烁 -> 20% 1绿5红 -> ... -> >90% 6绿
// 充电中：呼吸流动动画

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
    void setLed(uint8_t idx, uint8_t r, uint8_t g, uint8_t b);
    void showLeds();
    uint8_t _brightness;    // 呼吸动画用
    uint32_t _lastAnim;
    bool _animDir;
};

#endif // BATTERY_METER_ADDON_H_
