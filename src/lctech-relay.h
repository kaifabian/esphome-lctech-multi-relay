#ifndef LCTECH_RELAY_H_
#define LCTECH_RELAY_H_

#include "esphome.h"
using namespace esphome;

class LCTechMultiRelayBus : public Component, public UARTDevice {
  public:  
    const String LCTECH_INIT_COMMAND_SEQUENCE = "WIFI CONNECTED\nWIFI GOT IP\nAT+CIPMUX=1\nAT+CIPSERVER=1,8080\nAT+CIPSTO=360";
    const String LCTECH_AT_RESET_MSG = "AT+RST";
    
    static const size_t UART_BUFFER_SIZE = 32;

  protected:
    char _uart_message_buffer[UART_BUFFER_SIZE];
    int _uart_message_buffer_pos;
    
    // Based on <https://esphome.io/components/text_sensor/uart.html>
    int readline_internal()
    {
      uint8_t readch;
      int rpos;

      if(!this->available()) return -1;

      if((readch = this->read()) <= 0) return -1;
      
      switch (readch) {
        case '\r': // Ignore CR
          break;
        case '\n': // Return on LF
          rpos = this->_uart_message_buffer_pos;
          this->_uart_message_buffer_pos = 0;  // Reset position index ready for next time
          return rpos;
          break;
        default:
          if (this->_uart_message_buffer_pos < this->UART_BUFFER_SIZE - 1) {
            this->_uart_message_buffer[this->_uart_message_buffer_pos++] = readch;
            this->_uart_message_buffer[this->_uart_message_buffer_pos] = 0;
          }
          return -1;
      }
    }
    
    const String readline(void) {
      if (this->readline_internal() > 0) {
        return String(this->_uart_message_buffer);
      }
      return "";
    }

  public:
    LCTechMultiRelayBus(UARTComponent *parent): UARTDevice(parent), _uart_message_buffer_pos(0) {}
    
    void setup() override {
      delay(300);
    }

    void loop() override {
      const String message = this->readline();
      
      if(message.startsWith(this->LCTECH_AT_RESET_MSG)) {
        this->write_str(this->LCTECH_INIT_COMMAND_SEQUENCE.c_str());
      }
    }
    
    void set_relay_state(int index, bool state) {
      std::array<uint8_t, 4> msg;

      // Prepare command
      msg[0] = 0xa0;  // SET_RELAY_STATE command
      msg[1] = index; // 1-based relay index
      
      // Set desired state (0 == closed, 1 == open [default state])
      msg[2] = state ? 0 : 1;
      // Calculate simple checksum
      msg[3] = msg[0] + msg[1] + msg[2];

      for(int i = 0; i < 3; i++) {
        // Send twice with some delay since there's no ACK
        if(i) delay(10);
        this->write_array(msg);
      }
    }
};

class LCTechRelay : public Component, public Switch {
  protected:
    LCTechMultiRelayBus* m_hub;
    const int idx;

  public:
    LCTechRelay(LCTechMultiRelayBus* parent, int relay_index) : m_hub(parent), idx(relay_index) {}

    void setup() override {
      // Make sure we're synchronized initially and all relays are off
      this->write_state(false);
    }

    void write_state(bool state) override {
      this->m_hub->set_relay_state(this->idx, !state);
      this->publish_state(state);    
    }
};

#endif  // LCTECH_RELAY_H_
