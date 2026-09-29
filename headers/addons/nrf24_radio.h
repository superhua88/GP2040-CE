#ifndef NRF24_RADIO_H_
#define NRF24_RADIO_H_

// ===================================================================
// HITBOX 无线链路 · nRF24L01+ SPI 驱动（GP2040-CE / pico-sdk 版）
// 双端共用：TX 插件(发射) / RX 插件(接收) 都 include 这一份
// 引脚使用 GP2040-CE 的 PIN_MAP 命名空间约定，实际引脚在各自 addon.h 定义
// ===================================================================
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/gpio.h"
#include <stdint.h>
#include <string.h>

// ---- 链路参数（双端一致）----
#define RADIO_ADDR5       0xC2, 0xC2, 0xC2, 0xC2, 0xC2
#define RADIO_CHANNEL     76
#define RADIO_PAYLOAD_LEN 6
#define RADIO_LOST_MS     200   // 断流判卡键阈值

// ---- 键位 bit（与 radio_protocol.h 保持一致）----
enum radio_bits : uint8_t {
  RB_LB = 0, RB_RB, RB_HOME, RB_EMPTY,
  RB_A, RB_B, RB_X, RB_Y,
  RB_UP, RB_DOWN, RB_LEFT, RB_RIGHT,
  RB_START, RB_BACK, RB_LS, RB_RS,
  RB_LT, RB_RT,
};

typedef struct __attribute__((packed)) radio_packet_t {
  uint8_t magic;
  uint8_t buttons[3];
  uint8_t battery;
  uint8_t seq;
  uint8_t crc8;
} radio_packet_t;

static inline uint8_t radio_crc8(const uint8_t *d, int n) {
  uint8_t c = 0;
  while (n--) {
    c ^= *d++;
    for (int i = 0; i < 8; i++) c = (c & 1) ? (c >> 1) ^ 0x8C : (c >> 1);
  }
  return c;
}

static inline void radio_pack(radio_packet_t *p, uint32_t btnBits,
                              uint8_t battery, uint8_t seq) {
  p->magic = 0xA5;
  p->buttons[0] = (uint8_t)(btnBits);
  p->buttons[1] = (uint8_t)(btnBits >> 8);
  p->buttons[2] = (uint8_t)(btnBits >> 16);
  p->battery = battery;
  p->seq = seq;
  p->crc8 = radio_crc8((const uint8_t *)p, 5);
}

static inline bool radio_check(const radio_packet_t *p) {
  return p->magic == 0xA5 && p->crc8 == radio_crc8((const uint8_t *)p, 5);
}

// ---- nRF24L01+ 寄存器命令（SPI）----
#define NRF_CMD_R_REG     0x00
#define NRF_CMD_W_REG     0x20
#define NRF_CMD_R_RX_PW   0x60
#define NRF_CMD_W_TX_PW   0xA0
#define NRF_CMD_FLUSH_TX  0xE1
#define NRF_CMD_FLUSH_RX  0xE2
#define NRF_CMD_W_ACK     0xA8
#define NRF_CMD_ACTIVATE  0x50

#define NRF_REG_CONFIG    0x00
#define NRF_REG_EN_AA     0x01
#define NRF_REG_EN_RXADDR 0x02
#define NRF_REG_SETUP_AW  0x03
#define NRF_REG_SETUP_RETR 0x04
#define NRF_REG_RF_CH     0x05
#define NRF_REG_RF_SETUP  0x06
#define NRF_REG_STATUS    0x07
#define NRF_REG_RX_ADDR_P0 0x0A
#define NRF_REG_TX_ADDR   0x10
#define NRF_REG_RX_PW_P0  0x11
#define NRF_REG_FIFO_RX   0x61

class Nrf24Radio {
public:
  void init(spi_inst_t *spi, uint8_t sck, uint8_t mosi, uint8_t miso,
            uint8_t csn, uint8_t ce) {
    _spi = spi; _csn = csn; _ce = ce;
    spi_init(spi, 4000000);            // 4MHz，nRF24 SPI 上限 10MHz 留余量
    spi_set_format(spi, 8, SPI_CPOL_0, SPI_CPHA_0, SPI_MSB_FIRST);
    gpio_set_function(sck, GPIO_FUNC_SPI);
    gpio_set_function(mosi, GPIO_FUNC_SPI);
    gpio_set_function(miso, GPIO_FUNC_SPI);
    gpio_init(_csn);  gpio_set_dir(_csn, GPIO_OUT); gpio_put(_csn, 1);
    gpio_init(_ce);   gpio_set_dir(_ce, GPIO_OUT);  gpio_put(_ce, 0);
    sleep_ms(50);                       // 上电起振 1.5ms + 余量
    writeReg(NRF_REG_SETUP_AW, 0x03);   // 5 字节地址
    const uint8_t addr[5] = { RADIO_ADDR5 };
    writeBuf(NRF_REG_TX_ADDR | NRF_CMD_W_REG, addr, 5);
    writeBuf(NRF_REG_RX_ADDR_P0 | NRF_CMD_W_REG, addr, 5);
    writeReg(NRF_REG_EN_AA, 0x01);      // P0 自动应答
    writeReg(NRF_REG_EN_RXADDR, 0x01);  // 使能 P0
    writeReg(NRF_REG_SETUP_RETR, 0x13); // 500us x 3 重发
    writeReg(NRF_REG_RF_CH, RADIO_CHANNEL);
    writeReg(NRF_REG_RF_SETUP, 0x26);   // 2Mbps, 0dBm
    writeReg(NRF_REG_RX_PW_P0, RADIO_PAYLOAD_LEN);
    cmd(NRF_CMD_FLUSH_TX);
    cmd(NRF_CMD_FLUSH_RX);
    clearFlags();
  }

  // PTX 模式（发射端用）：true = 收到 ACK
  bool write(const void *data, uint8_t len) {
    writeReg(NRF_REG_CONFIG, 0x0E);     // PWR_UP | PRIM_TX | CRC 2byte
    cmd(NRF_CMD_FLUSH_TX);
    gpio_put(_ce, 0);
    cmd(NRF_CMD_W_TX_PW);
    spiWrite((const uint8_t *)data, len);
    gpio_put(_ce, 1);
    sleep_us(15);                       // CE 高电平 >10us 触发发送
    gpio_put(_ce, 0);
    // 等待发送完成（TX_DS 或 MAX_RT），最长 ~4ms（6 次重发耗时）
    for (int i = 0; i < 400; i++) {
      uint8_t st = readReg(NRF_REG_STATUS);
      if (st & 0x30) {                  // TX_DS(0x20) | MAX_RT(0x10)
        writeReg(NRF_REG_STATUS, st & 0x70); // 清标志
        return (st & 0x20) != 0;
      }
      sleep_us(10);
    }
    return false;
  }

  // PRX 模式（接收端用）：持续收
  void startRx() {
    writeReg(NRF_REG_CONFIG, 0x0F);     // PWR_UP | PRIM_RX | CRC 2byte
    gpio_put(_ce, 1);
    sleep_us(150);                      // 启动接收 130us
  }

  // 收一包（非阻塞）：true = 拿到一包
  bool read(void *out, uint8_t len) {
    uint8_t st = readReg(NRF_REG_STATUS);
    if (!(st & 0x40)) return false;     // RX_DR 未置位
    gpio_put(_csn, 0);
    spiWriteByte(NRF_CMD_R_RX_PW);      // 0x61: R_RX_PAYLOAD
    spiRead((uint8_t *)out, len);
    gpio_put(_csn, 1);
    writeReg(NRF_REG_STATUS, 0x40);     // 清 RX_DR
    return true;
  }

  // 写 ACK 载荷（PRX 侧，发给 PTX）
  void writeAckPayload(const void *data, uint8_t len) {
    gpio_put(_csn, 0);
    spiWriteByte(NRF_CMD_W_ACK);
    spiWrite((const uint8_t *)data, len);
    gpio_put(_csn, 1);
  }

private:
  spi_inst_t *_spi;
  uint8_t _csn, _ce;

  void spiWriteByte(uint8_t b) { spi_write_blocking(_spi, &b, 1); }
  void spiWrite(const uint8_t *p, uint8_t n) { spi_write_blocking(_spi, p, n); }
  void spiRead(uint8_t *p, uint8_t n) { spi_read_blocking(_spi, 0xFF, p, n); }

  void writeReg(uint8_t reg, uint8_t val) {
    gpio_put(_csn, 0);
    spiWriteByte(reg); spiWriteByte(val);
    gpio_put(_csn, 1);
  }
  void writeBuf(uint8_t reg, const uint8_t *p, uint8_t n) {
    gpio_put(_csn, 0);
    spiWriteByte(reg); spiWrite(p, n);
    gpio_put(_csn, 1);
  }
  uint8_t readReg(uint8_t reg) {
    gpio_put(_csn, 0);
    spiWriteByte(NRF_CMD_R_REG | reg);
    uint8_t v = 0; spiRead(&v, 1);
    gpio_put(_csn, 1);
    return v;
  }
  void cmd(uint8_t c) {
    gpio_put(_csn, 0); spiWriteByte(c); gpio_put(_csn, 1);
  }
  void clearFlags() { writeReg(NRF_REG_STATUS, 0x70); }
};

#endif // NRF24_RADIO_H_
