// ===================================================================
// GP2040-CE 插件 · nRF24 发射端实现（Hitbox 壳内的 Pico）
// process(): 读当前 GamepadState（主流程已完成 SOCD）-> 打包 nRF24 发送
// ===================================================================
#include "addons/nrf24tx.h"
#include "storagemanager.h"
#include "gamepad.h"

bool Nrf24TxAddon::available() {
    return NRF24_TX_ENABLED;
}

void Nrf24TxAddon::setup() {
    _radio.init(spi0, NRF24_PIN_SCK, NRF24_PIN_MOSI, NRF24_PIN_MISO,
                NRF24_PIN_CSN, NRF24_PIN_CE);
    memset(&_pkt, 0, sizeof(_pkt));
    _seq = 0;
    _lastHeartbeat = 0;
    _lastButtons = 0xFFFFFFFF;  // 强制首次发送
}

void Nrf24TxAddon::preprocess() {
    // 发送在 process()（输入状态已含 SOCD 结果），这里无需动作
}

void Nrf24TxAddon::process() {
    Gamepad &gamepad = Storage::getInstance().GetGamepad();
    GamepadState &s = gamepad.state;

    uint32_t bits = 0;
    auto set = [&](bool v, uint8_t bit) { if (v) bits |= (1UL << bit); };

    set(s.hasLB,    RB_LB);
    set(s.hasRB,    RB_RB);
    set(s.hasHome,  RB_HOME);
    set(gamepad.pressedB1(), RB_A);
    set(gamepad.pressedB2(), RB_B);
    set(gamepad.pressedB3(), RB_X);
    set(gamepad.pressedB4(), RB_Y);
    set(gamepad.pressedUp(),    RB_UP);
    set(gamepad.pressedDown(),  RB_DOWN);
    set(gamepad.pressedLeft(),  RB_LEFT);
    set(gamepad.pressedRight(), RB_RIGHT);
    set(s.hasStart, RB_START);
    set(s.hasBack,  RB_BACK);
    set(s.hasLS,    RB_LS);
    set(s.hasRS,    RB_RS);
    set(s.lt > GAMEPAD_TRIGGER_MIN, RB_LT);
    set(s.rt > GAMEPAD_TRIGGER_MIN, RB_RT);

    bool changed = (bits != _lastButtons);
    uint32_t now = getMillis();
    if (!changed && (now - _lastHeartbeat) < 50) return;

    radio_pack(&_pkt, bits, 100, _seq++);
    _radio.write(&_pkt, sizeof(_pkt));
    _lastButtons = bits;
    _lastHeartbeat = now;
}
