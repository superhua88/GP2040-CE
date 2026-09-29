#ifndef NRF24_RX_ADDON_H_
#define NRF24_RX_ADDON_H_

// ===================================================================
// GP2040-CE 鎻掍欢 路 nRF24 鎺ユ敹绔紙dongle 閭ｅ潡 Pico锛?// 浣滅敤锛氭敹鍙戝皠绔彂鏉ョ殑鎸夐敭鍖?-> 鍐欏叆 GamepadState锛堝啋鍏呮湁绾挎墜鏌勭粰涓绘満锛?// 鎸傝浇鐐癸細gp2040.cpp -> addons.LoadAddon(new Nrf24RxAddon(), CORE0_Input)
// ===================================================================
#include "gpaddon.h"
#include "enums.pb.h"
#include "BoardConfig.h"

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
  virtual void preprocess();   // 姣忓惊鐜細鏀跺寘 -> 鍐欏叆 gamepad state
  virtual void process();
  virtual std::string name() { return Nrf24RxAddonName; }

private:
  Nrf24Radio _radio;
  radio_packet_t _pkt;
  uint32_t _lastSeqTime;       // 鏈€杩戜竴娆℃湁鏁堝寘鏃堕棿
  bool _linkUp;                // 閾捐矾鏄惁娲荤潃
};

#endif // NRF24_RX_ADDON_H_
