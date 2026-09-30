// ===================================================================
// GP2040-CE 插件 · 电量灯条实现（NeoPico 驱动版）
// 电量来源：官方 Battery 插件写入的 auxState.sensor.battery (0-100)
// ===================================================================
#include "addons/batterymeter.h"
#include "storagemanager.h"
#include "gamepad.h"
#include "NeoPico.h"

static NeoPico *meterPico = nullptr;
static uint32_t frame[METER_LED_COUNT];

// GRB 打包（WS2812 标准）：(G<<16)|(R<<8)|B
static inline uint32_t grb(uint8_t r, uint8_t g, uint8_t b) {
    return ((uint32_t)g << 16) | ((uint32_t)r << 8) | b;
}

#define C_GREEN  grb(0, 255, 0)
#define C_RED    grb(255, 0, 0)
#define C_OFF    grb(0, 0, 0)

bool BatteryMeterAddon::available() {
    return BATTERY_METER_ENABLED;
}

void BatteryMeterAddon::setup() {
    _breath = 0;
    _breathDir = true;
    _lastAnim = 0;
    meterPico = new NeoPico();
    meterPico->Setup(METER_LED_PIN, METER_LED_COUNT, LED_FORMAT_GRB, pio0, 0);
}

void BatteryMeterAddon::process() {
    Gamepad *gamepad = Storage::getInstance().GetGamepad();
    uint8_t pct = gamepad->auxState.sensor.battery;

    uint32_t now = getMillis();
    bool charging = (gamepad->auxState.sensor.charging != 0);

    // 刷新节流：静态 500ms；充电动画 50ms
    uint32_t interval = charging ? 50 : 500;
    if (now - _lastAnim < interval) return;
    _lastAnim = now;

    // 绿灯数量梯度
    uint8_t greenCount;
    if      (pct >= 90) greenCount = 6;
    else if (pct >= 75) greenCount = 5;
    else if (pct >= 58) greenCount = 4;
    else if (pct >= 42) greenCount = 3;
    else if (pct >= 25) greenCount = 2;
    else if (pct >= 12) greenCount = 1;
    else                greenCount = 0;

    if (charging) {
        // 充电：已充的常绿，正在充的呼吸，其余灭
        for (uint8_t i = 0; i < METER_LED_COUNT; i++) {
            if (i < greenCount)            _frame[i] = C_GREEN;
            else if (i == greenCount)      _frame[i] = grb(0, _breath, 0);
            else                           _frame[i] = C_OFF;
        }
        if (_breathDir) { _breath += 5; if (_breath >= 250) _breathDir = false; }
        else            { _breath -= 5; if (_breath <= 5)   _breathDir = true;  }
    } else {
        bool blink = false;
        if (pct < 12) blink = ((now / 300) % 2) == 0;
        for (uint8_t i = 0; i < METER_LED_COUNT; i++) {
            if (pct < 12) {
                _frame[i] = blink ? C_RED : C_OFF;
            } else if (i < greenCount) {
                _frame[i] = C_GREEN;
            } else {
                _frame[i] = C_RED;
            }
        }
    }

    meterPico->SetFrame(_frame);
    meterPico->Show();
}
