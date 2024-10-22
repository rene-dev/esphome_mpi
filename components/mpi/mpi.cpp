#include "mpi.h"
#include "esphome/core/log.h"
#include "esphome/core/helpers.h"

namespace esphome {
namespace pipsolar {

static const char *const TAG = "mpi";

void Pipsolar::setup() {
    ESP_LOGD(TAG, "setup");
//   //this->state_ = STATE_IDLE;
//   //this->command_start_millis_ = 0;
this->read_pos = 0;
}

void Pipsolar::loop() {
    //ESP_LOGD(TAG, "loop");

}

void Pipsolar::empty_uart_buffer() {
    uint8_t byte;
    while (this->available()) {
        this->read_byte(&byte);
    }
}
//GS
//^D1107069,7079,0150,0154,0500,075,+00000,2300,2305,2311,4998,0000,0000,0000,0000,0000,0000,0000,,,,041,047,000,0
void Pipsolar::update() {
    ESP_LOGD(TAG, "update");
    
    while (this->available()) {
        this->read_byte(&this->read_buffer[this->read_pos]);
        this->read_pos++;
    }
    if(this->read_pos >= 3){// crc + \r
        this->read_buffer[this->read_pos-1] = '\0';
        ESP_LOGD(TAG, "rx len:%u, data:%s",this->read_pos, this->read_buffer);
        this->check_incoming_crc_();
    }

    this->read_pos = 0;
    this->empty_uart_buffer();
    this->write_str("^P003GS\r");
}

uint16_t Pipsolar::pipsolar_crc_(uint8_t *msg, uint8_t len) {
  uint16_t crc = crc16be(msg, len);
  uint8_t crc_low = crc & 0xff;
  uint8_t crc_high = crc >> 8;
  if (crc_low == 0x28 || crc_low == 0x0d || crc_low == 0x0a)
    crc_low++;
  if (crc_high == 0x28 || crc_high == 0x0d || crc_high == 0x0a)
    crc_high++;
  crc = (crc_high << 8) | crc_low;
  return crc;
}

uint8_t Pipsolar::check_incoming_crc_() {
  uint16_t crc16;
  crc16 = this->pipsolar_crc_(read_buffer, read_pos - 3);
  ESP_LOGD(TAG, "checking crc on incoming message");
  if (((uint8_t) ((crc16) >> 8)) == read_buffer[read_pos - 3] &&
      ((uint8_t) ((crc16) &0xff)) == read_buffer[read_pos - 2]) {
    ESP_LOGD(TAG, "CRC OK");
    read_buffer[read_pos - 1] = 0;
    read_buffer[read_pos - 2] = 0;
    read_buffer[read_pos - 3] = 0;
    return 1;
  }
  ESP_LOGD(TAG, "CRC NOK expected: %X %X but got: %X %X", ((uint8_t) ((crc16) >> 8)), ((uint8_t) ((crc16) &0xff)),
           read_buffer[read_pos - 3], read_buffer[read_pos - 2]);
  return 0;
}

void Pipsolar::dump_config() {
    ESP_LOGD(TAG, "dump_config");
}

}
}