// ===================================================================
// GP2040-CE 插件 · nRF24 发射端实现
// v2.1 适配单键电子开关（短按 toggle 型）：
//   用户短按模块键 -> 硬件通电/断电
//   固件自动关机（断链/闲置超时）-> pressSim() 经光耦模拟短按 -> 模块断电
//   插 USB 线（有线模式）时永不自动关机
// ===================================================================
#include "addons/nrf24tx.h"
#include "storagemanager.h"
#include "gamepad.h"
#include "tusb.h"   // tud_ready()

bool Nrf24TxAddon::available() {
    return NRF24_TX_ENABLED;
}

void Nrf24TxAddon::setup() {
    // 光耦控制脚：默认低（不触发），高电平 150ms = 模拟一次短按
    gpio_init(PRESS_SIM_PIN);
    gpio_set_dir(PRESS_SIM_PIN, GPIO_OUT);
    gpio_put(PRESS_SIM_PIN, 0);

    // 无线初始化
    _radio.init(spi0, NRF24_PIN_SCK, NRF24_PIN_MOSI, NRF24_PIN_MISO,
                NRF24_PIN_CSN, NRF24_PIN_CE);
    memset(&_pkt, 0, sizeof(_pkt));
    _seq = 0;
    _lastHeartbeat = 0;
    _lastButtons = 0xFFFFFFFF;
    _lastActivity = getMillis();
    _lastActivityBits = 0xFFFFFFFF;
    _lastLink = 0;
    _linkUp = false;
}

void Nrf24TxAddon::preprocess() {}

void Nrf24TxAddon::process() {
    Gamepad *gamepad = Storage::getInstance().GetGamepad();
    GamepadState &s = gamepad->state;
    uint32_t now = getMillis();

    // ---- 有线模式豁免：USB 枚举成功时跳过无线发包（ALWAYS_TX 调试模式除外）----
    if (tud_ready() && !NRF24_ALWAYS_TX) {
        _lastActivity = now;
        return;
    }

    // ---- 组包发送（50ms 心跳 / 输入变化立即发）----
    uint32_t bits = 0;
    auto set = [&](bool v, uint8_t bit) { if (v) bits |= (1UL << bit); };

    set(s.buttons & GAMEPAD_MASK_L1, RB_LB);
    set(s.buttons & GAMEPAD_MASK_R1, RB_RB);
    set(s.buttons & GAMEPAD_MASK_A1, RB_HOME);
    set(s.buttons & GAMEPAD_MASK_B1, RB_A);
    set(s.buttons & GAMEPAD_MASK_B2, RB_B);
    set(s.buttons & GAMEPAD_MASK_B3, RB_X);
    set(s.buttons & GAMEPAD_MASK_B4, RB_Y);
    set(s.dpad & GAMEPAD_MASK_UP,    RB_UP);
    set(s.dpad & GAMEPAD_MASK_DOWN,  RB_DOWN);
    set(s.dpad & GAMEPAD_MASK_LEFT,  RB_LEFT);
    set(s.dpad & GAMEPAD_MASK_RIGHT, RB_RIGHT);
    set(s.buttons & GAMEPAD_MASK_S2, RB_START);
    set(s.buttons & GAMEPAD_MASK_S1, RB_BACK);
    set(s.buttons & GAMEPAD_MASK_L3, RB_LS);
    set(s.buttons & GAMEPAD_MASK_R3, RB_RS);
    set(s.lt > GAMEPAD_TRIGGER_MIN, RB_LT);
    set(s.rt > GAMEPAD_TRIGGER_MIN, RB_RT);

    bool changed = (bits != _lastButtons);
    if (!changed && (now - _lastHeartbeat) < 50) {
        // 掉到超时判断
    } else {
        radio_pack(&_pkt, bits, 100, _seq++);
        bool acked = _radio.write(&_pkt, sizeof(_pkt));
        _lastButtons = bits;
        _lastHeartbeat = now;
        if (acked) {
            _linkUp = true;
            _lastLink = now;
        }
    }

    // ---- 活动时间戳 ----
    if (bits != _lastActivityBits) {
        _lastActivityBits = bits;
        _lastActivity = now;
    }

    // ---- 自动关机判断 ----
    // a. 开机后从未连上 dongle：给 60s 上线窗口 + 5min 宽限
    if (!_linkUp && _lastLink == 0 &&
        now > (60000 + POWER_LINK_LOST_OFF_MS)) {
        powerOff();
    }
    // b. 曾连上但断链超时
    if (_linkUp && (now - _lastLink) > POWER_LINK_LOST_OFF_MS) {
        powerOff();
    }
    // c. 无操作超时（按键位图长时间无变化）
    if (now - _lastActivity > POWER_IDLE_OFF_MS) {
        powerOff();
    }
}

// 经 PC817 光耦模拟一次"短按"：模块收到后 toggle 断电
void Nrf24TxAddon::pressSim() {
    gpio_put(PRESS_SIM_PIN, 1);
    sleep_ms(PRESS_PULSE_MS);
    gpio_put(PRESS_SIM_PIN, 0);
}

void Nrf24TxAddon::powerOff() {
    pressSim();
    // 模块断电有几十 ms 延迟，死循环等待物理断电
    while (true) {
        tight_loop_contents();
    }
}
