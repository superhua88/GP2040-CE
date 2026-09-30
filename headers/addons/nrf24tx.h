#ifndef NRF24_TX_ADDON_H_
#define NRF24_TX_ADDON_H_

// ===================================================================
// GP2040-CE 插件 · nRF24 发射端（Hitbox 壳内的 Pico）
// v2: 增加软开关电源管理（点按开机 / 长按关机 / 断链超时关机 / 闲置超时关机）
// ===================================================================
#include "gpaddon.h"
#include "enums.pb.h"

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

// ---- 软开关电源管理引脚 ----
// POWER_HOLD: 接一键开关机模块的 EN/KEY 脚（高=保持供电，低=断电）
// POWER_BTN : 接轻触按钮另一组触点（按下=低，内部上拉；用于长按关机检测）
#ifndef POWER_HOLD_PIN
#define POWER_HOLD_PIN 7
#endif
#ifndef POWER_BTN_PIN
#define POWER_BTN_PIN  8
#endif

// ---- 自动断电阈值（毫秒）----
#ifndef POWER_LINK_LOST_OFF_MS
#define POWER_LINK_LOST_OFF_MS 300000UL   // 断链 5 分钟 -> 关机
#endif
#ifndef POWER_IDLE_OFF_MS
#define POWER_IDLE_OFF_MS 1800000UL       // 无操作 30 分钟 -> 关机
#endif
#ifndef POWER_BTN_HOLD_OFF_MS
#define POWER_BTN_HOLD_OFF_MS 2000UL      // 长按 2 秒 -> 手动关机
#endif

#include "nrf24_radio.h"

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
    uint32_t _lastActivity;    // 最近一次输入变化时间
    uint32_t _lastActivityBits;
    uint32_t _lastLink;        // 最近一次收到 ACK 的时间
    uint32_t _btnPressStart;   // 按钮按下起始时间（0=未按）
    bool _linkUp;
    void powerOff();
};

#endif // NRF24_TX_ADDON_H_
