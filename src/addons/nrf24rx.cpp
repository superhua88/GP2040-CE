// ===================================================================
// GP2040-CE 插件 · nRF24 接收端实现（dongle 的 Pico）
// preprocess(): 收包 -> 写 GamepadState（方向用 GAMEPAD_MASK 方向位）
// 断流 >RADIO_LOST_MS 无有效包 -> 清空按键（防卡键）
// ===================================================================
#include "addons/nrf24rx.h"
#include "storagemanager.h"
#include "gamepad.h"

bool Nrf24RxAddon::available() {
    return NRF24_RX_ENABLED;
}

void Nrf24RxAddon::setup() {
    _radio.init(spi0, NRF24_RX_PIN_SCK, NRF24_RX_PIN_MOSI, NRF24_RX_PIN_MISO,
                NRF24_RX_PIN_CSN, NRF24_RX_PIN_CE);
    _radio.startRx();
    memset(&_pkt, 0, sizeof(_pkt));
    _lastSeqTime = 0;
    _linkUp = false;
    uint8_t status = 0x01;
    _radio.writeAckPayload(&status, 1);
}

void Nrf24RxAddon::preprocess() {
    Gamepad &gamepad = Storage::getInstance().GetGamepad();
    GamepadState &s = gamepad.state;

    bool got = _radio.read(&_pkt, sizeof(_pkt));
    if (got && radio_check(&_pkt)) {
        _linkUp = true;
        _lastSeqTime = getMillis();
        uint8_t status = 0x01;
        _radio.writeAckPayload(&status, 1);

        uint32_t bits = _pkt.buttons[0] | (_pkt.buttons[1] << 8) | (_pkt.buttons[2] << 16);
        auto on = [&](uint8_t bit) { return bits & (1UL << bit); };

        // 发射端已完成 SOCD，这里直接透传（clearState 后重建）
        gamepad.clearState();
        s.hasLB    = on(RB_LB);
        s.hasRB    = on(RB_RB);
        s.hasHome  = on(RB_HOME);
        s.hasStart = on(RB_START);
        s.hasBack  = on(RB_BACK);
        s.hasLS    = on(RB_LS);
        s.hasRS    = on(RB_RS);
        s.hasLT    = on(RB_LT);
        s.hasRT    = on(RB_RT);
        s.lt = s.hasLT ? GAMEPAD_TRIGGER_MAX : GAMEPAD_TRIGGER_MIN;
        s.rt = s.hasRT ? GAMEPAD_TRIGGER_MAX : GAMEPAD_TRIGGER_MIN;
        if (on(RB_A)) s.buttons |= GAMEPAD_MASK_B1;
        if (on(RB_B)) s.buttons |= GAMEPAD_MASK_B2;
        if (on(RB_X)) s.buttons |= GAMEPAD_MASK_B3;
        if (on(RB_Y)) s.buttons |= GAMEPAD_MASK_B4;
        if (on(RB_UP))    s.dpad |= GAMEPAD_MASK_UP;
        if (on(RB_DOWN))  s.dpad |= GAMEPAD_MASK_DOWN;
        if (on(RB_LEFT))  s.dpad |= GAMEPAD_MASK_LEFT;
        if (on(RB_RIGHT)) s.dpad |= GAMEPAD_MASK_RIGHT;
        s.dpadOriginal = s.dpad;
    }

    // 断流保护
    uint32_t now = getMillis();
    if (_linkUp && (now - _lastSeqTime > RADIO_LOST_MS)) {
        _linkUp = false;
        gamepad.clearState();
    }
}

void Nrf24RxAddon::process() {
    // 无附加动作
}
