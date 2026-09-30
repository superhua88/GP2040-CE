// ===================================================================
// GP2040-CE 插件 · 电量灯条实现（NeoPico 驱动版）
// 电量来源：GamepadAuxPower（auxState.power.level 0-100 / .charging）
// 由 Pico 电池分压 ADC 自行采样（本插件自带采样，无需官方 Battery 插件）
// ===================================================================
#include "addons/batterymeter.h"
#include "storagemanager.h"
#include "gamepad.h"
#include "NeoPico.h"
#include "hardware/adc.h"

static NeoPico *meterPico = nullptr;
static uint32_t frame[METER_LED_COUNT];

#ifndef BATTERY_ADC_PIN
#define BATTERY_ADC_PIN 26       // ADC0：接电池分压中点
#endif
#ifndef BATTERY_ADC_FULL_MV
#define BATTERY_ADC_FULL_MV 4200 // 满电电压
#endif
#ifndef BATTERY_ADC_EMPTY_MV
#define BATTERY_ADC_EMPTY_MV 3300 // 建议截止电压（留保护余量）
#endif
// 分压系数：100k/100k 分压时 ADC 读数是电池电压一半，x2 还原；按实际电阻改
#ifndef BATTERY_DIVIDER
#define BATTERY_DIVIDER 2.0f
#endif

// GRB 打包（WS2812 标准）
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

    // ADC 采样电池分压
    adc_init();
    adc_gpio_init(BATTERY_ADC_PIN);
    adc_select_input(BATTERY_ADC_PIN - 26);
}

uint8_t BatteryMeterAddon::readBatteryPercent() {
    // 12bit ADC，Vref=3.3V；分压还原
    uint16_t raw = adc_read();
    uint32_t mv = (uint32_t)(raw * 3300UL / 4095) * BATTERY_DIVIDER;
    if (mv >= BATTERY_ADC_FULL_MV) return 100;
    if (mv <= BATTERY_ADC_EMPTY_MV) return 0;
    return (uint8_t)((mv - BATTERY_ADC_EMPTY_MV) * 100UL / (BATTERY_ADC_FULL_MV - BATTERY_ADC_EMPTY_MV));
}

void BatteryMeterAddon::process() {
    Gamepad *gamepad = Storage::getInstance().GetGamepad();
    uint8_t pct = readBatteryPercent();
    // 充电检测：插着 USB（VBUS 有电）即为充电/有线状态
    bool charging = (gamepad->auxState.power.pluggedIn != 0);

    uint32_t now = getMillis();
    uint32_t interval = charging ? 50 : 500;
    if (now - _lastAnim < interval) return;
    _lastAnim = now;

    uint8_t greenCount;
    if      (pct >= 90) greenCount = 6;
    else if (pct >= 75) greenCount = 5;
    else if (pct >= 58) greenCount = 4;
    else if (pct >= 42) greenCount = 3;
    else if (pct >= 25) greenCount = 2;
    else if (pct >= 12) greenCount = 1;
    else                greenCount = 0;

    if (charging) {
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
