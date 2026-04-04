#pragma once

#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"
#include "esphome/components/spi/spi.h"

namespace esphome {
namespace weact_epaper {

class WeActEPaper : public display::DisplayBuffer,
                    public spi::SPIDevice<spi::BIT_ORDER_MSB_FIRST,
                                         spi::CLOCK_POLARITY_LOW,
                                         spi::CLOCK_PHASE_LEADING,
                                         spi::DATA_RATE_2MHZ> {
 public:
  void set_cs_pin(int pin) { cs_pin_ = pin; }
  void set_dc_pin(int pin) { dc_pin_ = pin; }
  void set_busy_pin(int pin) { busy_pin_ = pin; }
  void set_reset_pin(int pin) { reset_pin_ = pin; }

  void setup() override;
  void update() override;
  void dump_config() override;
  float get_setup_priority() const override { return setup_priority::PROCESSOR; }

  int get_width() const override { return 416; }
  int get_height() const override { return 240; }

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override;

  void command(uint8_t cmd);
  void data(uint8_t data);
  void wait_until_idle();
  void reset();
  void init_display();
  void deep_sleep();
  void display_frame(const uint8_t* buffer);

  int cs_pin_;
  int dc_pin_;
  int busy_pin_;
  int reset_pin_;
  bool is_sleeping_{false};
};

}  // namespace weact_epaper
}  // namespace esphome