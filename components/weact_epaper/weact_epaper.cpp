#include "weact_epaper.h"
#include "esphome/core/log.h"
#include "esphome/core/hal.h"

namespace esphome {
namespace weact_epaper {

static const char *TAG = "weact_epaper";

void WeActEPaper::setup() {
  ESP_LOGCONFIG(TAG, "Setting up WeAct 3.7\" E-Paper (UC8253)...");

  // Reset display
  auto reset = make_optional(GPIO_OutputPin(reset_pin_));
  reset->pin_mode(gpio::FLAG_OUTPUT);
  reset->digital_write(true);
  delay(1);
  reset->digital_write(false);
  delay(5);
  reset->digital_write(true);
  delay(1);

  wait_until_idle();
  init_display();
}

void WeActEPaper::wait_until_idle() {
  auto busy = make_optional(GPIO_InputPin(busy_pin_));
  busy->pin_mode(gpio::FLAG_INPUT);

  int timeout = 1000;
  while (timeout-- && busy->digital_read()) {
    delay(1);
  }

  if (timeout <= 0) {
    ESP_LOGW(TAG, "Timeout waiting for display to become idle");
  }
}

void WeActEPaper::command(uint8_t cmd) {
  auto cs = make_optional(GPIO_OutputPin(cs_pin_));
  auto dc = make_optional(GPIO_OutputPin(dc_pin_));

  dc->digital_write(false);
  cs->digital_write(false);
  this->write_byte(cmd);
  cs->digital_write(true);
}

void WeActEPaper::data(uint8_t data) {
  auto cs = make_optional(GPIO_OutputPin(cs_pin_));
  auto dc = make_optional(GPIO_OutputPin(dc_pin_));

  dc->digital_write(true);
  cs->digital_write(false);
  this->write_byte(data);
  cs->digital_write(true);
}

void WeActEPaper::init_display() {
  ESP_LOGI(TAG, "Initializing UC8253 controller");

  command(0x00);  // Panel setting
  data(0xD7);
  data(0x0E);

  command(0x50);  // VCOM register
  data(0x47);

  command(0x04);  // Power on
  wait_until_idle();

  command(0x02);  // Enable display
  wait_until_idle();
}

void WeActEPaper::display_frame(const uint8_t* buffer) {
  ESP_LOGD(TAG, "Updating display frame");

  command(0x10);  // Data start
  wait_until_idle();

  // Send all pixels (416 * 240 / 8 = 12480 bytes)
  for (int i = 0; i < 12480; i++) {
    data(buffer[i]);
  }

  command(0x04);  // Power on
  wait_until_idle();

  command(0x12);  // Display refresh
  delay(10);
  wait_until_idle();

  command(0x02);  // Power off
  wait_until_idle();

  // Deep sleep
  command(0x07);
  data(0xA5);
}

void WeActEPaper::update() {
  this->display_frame(this->get_buffer());
}

void WeActEPaper::draw_absolute_pixel_internal(int x, int y, Color color) {
  if (x >= get_width() || y >= get_height() || x < 0 || y < 0)
    return;

  uint32_t pos = (y * get_width() + x);
  uint8_t* buffer = this->get_buffer();

  if (color.is_on()) {
    buffer[pos / 8] |= (1 << (7 - (pos % 8)));
  } else {
    buffer[pos / 8] &= ~(1 << (7 - (pos % 8)));
  }
}

void WeActEPaper::dump_config() {
  ESP_LOGCONFIG(TAG, "WeAct 3.7\" E-Paper Display");
  ESP_LOGCONFIG(TAG, "  Width: %d, Height: %d", get_width(), get_height());
  ESP_LOGCONFIG(TAG, "  CS Pin: %d", cs_pin_);
  ESP_LOGCONFIG(TAG, "  DC Pin: %d", dc_pin_);
  ESP_LOGCONFIG(TAG, "  BUSY Pin: %d", busy_pin_);
  ESP_LOGCONFIG(TAG, "  RESET Pin: %d", reset_pin_);
}

}  // namespace weact_epaper
}  // namespace esphome