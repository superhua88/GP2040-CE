#ifndef NRF24_TX_ADDON_H_
#define NRF24_TX_ADDON_H_

// ===================================================================
// GP2040-CE 鎻掍欢 路 nRF24 鍙戝皠绔紙鏀惧湪 Hitbox 澹冲唴鐨勯偅鍧?Pico锛?// 浣滅敤锛氭妸鏈湴杈撳叆灞傜姸鎬佹墦鍖咃紝缁?nRF24L01+ 鍙戠粰鎺ユ敹 dongle
// 鎸傝浇鐐癸細gp2040.cpp -> addons.LoadAddon(new Nrf24TxAddon(), CORE0_Input)
// ===================================================================
#include "gpaddon.h"
#include "enums.pb.h"
#include "BoardConfig.h"

#ifndef NRF24_TX_ENABLED
#define NRF24_TX_ENABLED 0
#endif

// SPI 寮曡剼锛圥ico 榛樿 SPI0 缁勶紝閬垮紑 USB/flash 涓撶敤鑴氾紱鎸夊疄闄呮帴绾挎敼锛?#ifndef NRF24_PIN_SCK
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
  virtual std::string name() { return Nrf24TxAddonName; }

private:
  Nrf24Radio _radio;
  radio_packet_t _pkt;
  uint8_t _seq;
  uint32_t _lastHeartbeat;
  uint32_t _buttons;
  uint32_t _lastButtons = 0;
};

#endif // NRF24_TX_ADDON_H_
