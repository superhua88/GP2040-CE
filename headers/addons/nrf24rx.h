#ifndef NRF24_RX_ADDON_H_
#define NRF24_RX_ADDON_H_

// ===================================================================
// GP2040-CE 插件 · nRF24 接收端（dongle 的 Pico）
// ===================================================================
#include "gpaddon.h"
#include "enums.pb.h"

#ifndef NRF24_RX_ENABLED
#define NRF24_RX_ENABLED 0
#endif

#ifndef NRF24_RX_PIN_SCK
#define NRF24_RX_PIN_SCK  2
#endif
#ifndef NRF24_RX_PIN_MOSI
#define NRF24_RX_PIN_MOSI 3
#endif
#ifndef NRF24_RX_PIN_MISO
#define NRF24_RX_PIN_MISO 4
#endif
#ifndef NRF24_RX_PIN_CSN
#define NRF24_RX_PIN_CSN  5
#endif
#ifndef NRF24_RX_PIN_CE
#define NRF24_RX_PIN_CE   6
#endif

#include "nrf24_radio.h"

#define Nrf24RxAddonName "NRF24 RX"

class Nrf24RxAddon : public GPAddon {
public:
    virtual bool available();
    virtual void setup();
    virtual void preprocess();
    virtual void process();
    virtual void postprocess(bool sent) {}
    virtual void reinit() {}
    virtual std::string name() { return Nrf24RxAddonName; }

private:
    Nrf24Radio _radio;
    radio_packet_t _pkt;
    uint32_t _lastSeqTime;
    bool _linkUp;
};

#endif // NRF24_RX_ADDON_H_
