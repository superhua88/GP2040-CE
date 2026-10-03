#ifndef NRF24_TX_ADDON_H_
#define NRF24_TX_ADDON_H_

// ===================================================================
// GP2040-CE 插件 · nRF24 发射端（Hitbox 壳内的 Pico）
// v2.1: 适配"单键电子开关"（短按 toggle 型，无 EN 脚）
//   - 开机：用户短按模块按键（硬件级，固件不参与）
//   - 手动关机：用户再短按（硬件级瞬时断电）
//   - 自动关机（断链/闲置超时）：固件经 PC817 光耦模拟一次短按
// ===================================================================
#include "gpaddon.h"
#include "enums.pb.h"

// 联调模式：1 = 即使插着 USB 也持续发包（开发期用）；正式版改 0
#ifndef NRF24_ALWAYS_TX
#define NRF24_ALWAYS_TX 1
#endif

#ifndef NRF24_TX_ENABLED
#define NRF24_TX_ENABLED 0
#endif

// ---- nRF24 SPI 引脚 ----
#ifndef NRF24_PIN_SCK
#define NRF24_PIN_SCK  2
#endif
#ifndef NRF24_PIN_MOSI
#define NRF24_PIN_MOSI 3
#endif
#ifndef NRF24_PIN_MISO
#define NRF24_PIN_MISO 4
#endif
#ifndef NRF24_PIN_CSN
#define NRF24_PIN_CSN  5
#endif
#ifndef NRF24_PIN_CE
#define NRF24_PIN_CE   6
#endif

// ---- 电源管理引脚 ----
// PRESS_SIM_PIN: 接 PC817 光耦 LED 侧（经限流电阻），输出侧跨接模块的 K+/K- 焊盘
// 高电平 100ms = 模拟一次短按 = 模块 toggle 断电
#ifndef PRESS_SIM_PIN
#define PRESS_SIM_PIN 7
#endif

// ---- 自动断电阈值（毫秒）----
#ifndef POWER_LINK_LOST_OFF_MS
#define POWER_LINK_LOST_OFF_MS 300000UL   // 断链 5 分钟 -> 自动关机
#endif
#ifndef POWER_IDLE_OFF_MS
#define POWER_IDLE_OFF_MS 1800000UL       // 无操作 30 分钟 -> 自动关机
#endif
#define PRESS_PULSE_MS 150                 // 模拟短按的脉宽

#include "nrf24_radio.h"

// Battery meter LED gauge (6x WS2812B on GP15)
#define BATTERY_METER_ENABLED 1

#define Nrf24TxAddonName "NRF24 TX"

class Nrf24TxAddon : public GPAddon {
public:
    virtual bool available();
    virtual void setup();
    virtual void preprocess();
    virtual void process();
    virtual void postprocess(bool sent) {}
    virtual void reinit() {}
    virtual std::string name() { return Nrf24TxAddonName; }

private:
    Nrf24Radio _radio;
    radio_packet_t _pkt;
    uint8_t _seq;
    uint32_t _lastHeartbeat;
    uint32_t _lastButtons;
    uint32_t _lastActivity;
    uint32_t _lastActivityBits;
    uint32_t _lastLink;
    bool _linkUp;
    void pressSim();
    void powerOff();
};

#endif // NRF24_TX_ADDON_H_
