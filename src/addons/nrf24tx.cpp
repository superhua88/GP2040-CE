// ===================================================================
// GP2040-CE 插件 · nRF24 发射端实现
// v2 软开关电源管理：
//   setup()   第一时间拉高 POWER_HOLD（取代开机按钮的保持信号）
//   process() 检测长按关机 / 断链超时 / 闲置超时 -> 释放 POWER_HOLD 断电
// ===================================================================
#include "addons/nrf24tx.h"
#include "storagemanager.h"
#include "gamepad.h"
#include "tusb.h"   // tud_ready()

bool Nrf24TxAddon::available() {
    return NRF24_TX_ENABLED;
}

void Nrf24TxAddon::setup() {
    // 电源保持：上电后立刻接管按钮的保持信号（按钮模块电路：EN 低有效断电）
    gpio_init(POWER_HOLD_PIN);
    gpio_set_dir(POWER_HOLD_PIN, GPIO_OUT);
    gpio_put(POWER_HOLD_PIN, 1);

    // 关机按钮：输入上拉，按下=低
    gpio_init(POWER_BTN_PIN);
    gpio_set_dir(POWER_BTN_PIN, GPIO_IN);
    gpio_pull_up(POWER_BTN_PIN);
    _btnPressStart = 0;

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

    // ---- 1. 长按关机检测（按住 2 秒）----
    bool btnDown = (gpio_get(POWER_BTN_PIN) == 0);
    if (btnDown) {
        if (_btnPressStart == 0) {
            _btnPressStart = now;
        } else if (now - _btnPressStart >= POWER_BTN_HOLD_OFF_MS) {
            powerOff();                     // 释放 POWER_HOLD -> 整机断电
        }
    } else {
        _btnPressStart = 0;
    }

    // ---- 2. 有线模式豁免：USB 已枚举时永不自动关机 ----
    if (tud_ready()) {
        _lastActivity = now;                // 有线使用也算"活动"
        return;                             // 跳过自动断电判断，但无线包也不必发
    }

    // ---- 3. 组包发送 ----
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
        // 无新包也继续超时判断（下方）
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

    // ---- 4. 活动时间戳 ----
    if (bits != _lastActivityBits || changed) {
        _lastActivityBits = bits;
        _lastActivity = now;
    }

    // ---- 5. 自动断电判断 ----
    // 5a. 从未连上过（开机后 60s 内没有 dongle 应答）且已超 5 分钟 -> 关机
    if (!_linkUp && now > POWER_LINK_LOST_OFF_MS && (now - _lastLink) > POWER_LINK_LOST_OFF_MS) {
        // 从未连上：_lastLink=0，now>5min 即可关（给 dongle 足够上线时间）
        if (_lastLink == 0 && now > POWER_LINK_LOST_OFF_MS) {
            powerOff();
        }
    }
    // 5b. 曾连上但断链超时
    if (_linkUp && (now - _lastLink) > POWER_LINK_LOST_OFF_MS) {
        powerOff();
    }
    // 5c. 无操作超时（不论链路状态）
    if (now - _lastActivity > POWER_IDLE_OFF_MS) {
        powerOff();
    }
}

void Nrf24TxAddon::powerOff() {
    // 释放保持信号：一键开关机模块的 MOS 断开，整机断电（功耗归零）
    gpio_put(POWER_HOLD_PIN, 0);
    // 断电是异步生效的（几十 ms 内），这里死循环等待断电，避免固件继续跑
    while (true) {
        tight_loop_contents();
    }
}
