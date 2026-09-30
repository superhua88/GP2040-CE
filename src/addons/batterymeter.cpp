// ===================================================================
// GP2040-CE 插件 · 电量灯条实现
// 数据来源：GP2040-CE Battery 插件的 auxState.sensor.battery 为 0-100
// （固件构建时需同时启用 Battery 插件，其 ADC 引脚接电池分压）
// ===================================================================
#include "addons/batterymeter.h"
#include "storagemanager.h"
#include "picoled.h"        // PicoLed 库（GP2040-CE 自带）
#include "hardware/adc.h"

bool BatteryMeterAddon::available() {
    return BATTERY_METER_ENABLED;
}

void BatteryMeterAddon::setup() {
    _brightness = 0;
    _lastAnim = 0;
    _animDir = true;
}

void BatteryMeterAddon::setLed(uint8_t idx, uint8_t r, uint8_t g, uint8_t b) {
    if (idx >= METER_LED_COUNT) return;
    PicoLed::setPixelColor(idx, r, g, b);
}

void BatteryMeterAddon::showLeds() {
    PicoLed::show();
}

void BatteryMeterAddon::process() {
    // 读电量（Battery 插件维护，0-100）
    Gamepad *gamepad = Storage::getInstance().GetGamepad();
    uint8_t pct = gamepad->auxState.sensor.battery;
    bool charging = gamepad->auxState.sensor.charging;   // Battery 插件可报充电状态

    uint32_t now = getMillis();
    // 动画节流：非充电 500ms 刷新一次；充电呼吸动画 50ms 一次
    if (!charging && (now - _lastAnim) < 500) return;
    if (charging && (now - _lastAnim) >= 50) { _lastAnim = now; }
    else if (!charging) { _lastAnim = now; }

    // 计算亮绿数量（每 17% 一颗：100%->6, 83%->5, ... 17%->1, <17%->0）
    uint8_t greenCount;
    if      (pct >= 90) greenCount = 6;
    else if (pct >= 75) greenCount = 5;
    else if (pct >= 58) greenCount = 4;
    else if (pct >= 42) greenCount = 3;
    else if (pct >= 25) greenCount = 2;
    else if (pct >= 12) greenCount = 1;
    else greenCount = 0;

    if (charging) {
        // ============ 充电动画：绿灯从左到右流动 ============
        // 亮起的位置数 = 已充比例对应的灯数，当前"正在充"的那颗做呼吸
        // 例：45% -> 3 绿常亮 + 第 4 颗呼吸
        for (uint8_t i = 0; i < METER_LED_COUNT; i++) {
            if (i < greenCount) {
                setLed(i, C_GREEN);                       // 已充好的常绿
            } else if (i == greenCount && greenCount < METER_LED_COUNT) {
                // 正在充的灯：呼吸（亮度渐变）
                uint8_t br = _brightness;
                setLed(i, 0, br, 0);
            } else {
                setLed(i, C_OFF);
            }
        }
        // 呼吸步进
        if (_animDir) { _brightness += 5; if (_brightness >= 250) _animDir = false; }
        else          { _brightness -= 5; if (_brightness <= 5)   _animDir = true;  }
        showLeds();
        return;
    }

    // ============ 非充电：静态电量显示 ============
    // 绿灯 = 已有电量；红色 = 已消耗部分；<12% 时全部红灯闪烁警告
    bool blink = false;
    if (pct < 12) {
        blink = ((now / 300) % 2) == 0;    // 300ms 闪烁
    }

    for (uint8_t i = 0; i < METER_LED_COUNT; i++) {
        if (pct < 12) {
            // 低电量警告：全部红灯闪烁
            if (blink) setLed(i, C_RED);
            else       setLed(i, C_OFF);
        } else if (i < greenCount) {
            setLed(i, C_GREEN);                // 剩余电量
        } else {
            setLed(i, C_RED);                  // 已消耗部分
        }
    }
    showLeds();
}
