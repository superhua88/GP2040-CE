// ===================================================================
// GP2040-CE 插件 · nRF24 发射端实现
// process(): 读 GamepadState（主流程已 SOCD）-> 打包 nRF24 发送
// GamepadState 无 hasXX 布尔，用 GAMEPAD_MASK 位判断
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
    _lastButtons = 0xFFFFFFFF;
}

void Nrf24TxAddon::preprocess() {}

void Nrf24TxAddon::process() {
    Gamepad *gamepad = Storage::getInstance().GetGamepad();
    GamepadState &s = gamepad->state;

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
    uint32_t now = getMillis();
    if (!changed && (now - _lastHeartbeat) < 50) return;

    radio_pack(&_pkt, bits, 100, _seq++);
    _radio.write(&_pkt, sizeof(_pkt));
    _lastButtons = bits;
    _lastHeartbeat = now;
}
