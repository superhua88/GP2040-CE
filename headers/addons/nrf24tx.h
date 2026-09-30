#ifndef NRF24_TX_ADDON_H_
#define NRF24_TX_ADDON_H_

// ===================================================================
// GP2040-CE 插件 · nRF24 发射端（Hitbox 壳内的 Pico）
// ===================================================================
#include "gpaddon.h"
#include "enums.pb.h"

#ifndef NRF24_TX_ENABLED
#define NRF24_TX_ENABLED 0
#endif

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
    uint32_t _buttons;
    uint32_t _lastButtons;
};

#endif // NRF24_TX_ADDON_H_
