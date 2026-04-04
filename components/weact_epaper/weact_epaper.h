#pragma once
#include "esphome/core/component.h"
#include "esphome/components/display/display_buffer.h"

namespace esphome {
namespace weact_epaper {

class WeActEPaper : public display::DisplayBuffer {
 public:
  void set_cs_pin(int pin) { cs_pin_ = pin; }
  void set_dc_pin(int pin) { dc_pin_ = pin; }
  void set_busy_pin(int pin) { busy_pin_ = pin; }
  void set_reset_pin(int pin) { reset_pin_ = pin; }
  void set_rotation(int rotation) { rotation_ = rotation; }
  void set_buffer_size(int size) { buffer_.resize(size, 0xFF); }
  
  void set_writer(std::function<void(display::DisplayBuffer &)> &&writer) { 
    writer_ = std::move(writer); 
  }

  void setup() override {
    ESP_LOGI("weact", "=== SETUP START ===");
    
    pinMode(cs_pin_, OUTPUT);
    pinMode(dc_pin_, OUTPUT);
    pinMode(reset_pin_, OUTPUT);
    pinMode(busy_pin_, INPUT);
    pinMode(6, OUTPUT);  // CLK
    pinMode(7, OUTPUT);  // MOSI
    
    digitalWrite(cs_pin_, HIGH);
    digitalWrite(dc_pin_, HIGH);
    digitalWrite(reset_pin_, HIGH);
    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);
    
    // Сброс дисплея
    ESP_LOGI("weact", "Resetting display...");
    digitalWrite(reset_pin_, LOW);
    delay(20);
    digitalWrite(reset_pin_, HIGH);
    delay(200);
    
    // Инициализация UC8253
    ESP_LOGI("weact", "Initializing UC8253...");
    this->write_command(0x00);  // panel setting
    this->write_data(0xD7);
    this->write_data(0x0E);
    
    this->write_command(0x50);
    this->write_data(0x47);
    
    // Очистка экрана (черный)
    ESP_LOGI("weact", "Clearing display...");
    for (auto &byte : buffer_) byte = 0xFF;
    this->send_buffer();
    
    ESP_LOGI("weact", "=== SETUP DONE ===");
  }

  void update() override {
    ESP_LOGI("weact", "=== UPDATE START ===");
    
    // Очищаем буфер перед рисованием (белый фон)
    for (auto &byte : buffer_) byte = 0xFF;
    
    // Вызываем lambda из YAML
    if (writer_) {
        writer_(*this);
        ESP_LOGI("weact", "Lambda executed");
    }
    
    this->send_buffer();
    ESP_LOGI("weact", "=== UPDATE DONE ===");
}

  void loop() override {}

  float get_setup_priority() const override { return setup_priority::PROCESSOR; }
  int get_width_internal() override { return 416; }
  int get_height_internal() override { return 240; }
  display::DisplayType get_display_type() override { return display::DisplayType::DISPLAY_TYPE_BINARY; }

 protected:
  void draw_absolute_pixel_internal(int x, int y, Color color) override {
    int width = get_width_internal();
    int height = get_height_internal();
    int new_x = x;
    int new_y = y;
    
    // Применяем поворот
    switch (rotation_) {
        case 0:  // 0°
            new_x = x;
            new_y = y;
            break;
        case 1:  // 90°
            new_x = y;
            new_y = height - 1 - x;
            break;
        case 2:  // 180°
            new_x = width - 1 - x;
            new_y = height - 1 - y;
            break;
        case 3:  // 270°
            new_x = width - 1 - y;
            new_y = x;
            break;
    }
    
    if (new_x >= 416 || new_x < 0 || new_y >= 240 || new_y < 0) return;
    
    int col = new_x;
    int row_byte = new_y / 8;
    int bit = new_y % 8;
    
    uint32_t pos = (row_byte * 416) + col;
    
    if (pos < buffer_.size()) {
        uint8_t bit_mask = 1 << (7 - bit);
        if (color.is_on()) {
            buffer_[pos] &= ~bit_mask;
        } else {
            buffer_[pos] |= bit_mask;
        }
    }
}

  void write_command(uint8_t cmd) {
    this->enable();
    digitalWrite(dc_pin_, LOW);
    this->spi_write_byte(cmd);
    this->disable();
  }
  
  void write_data(uint8_t data) {
    this->enable();
    digitalWrite(dc_pin_, HIGH);
    this->spi_write_byte(data);
    this->disable();
  }

 void send_buffer() {
    ESP_LOGI("weact", "Sending %d bytes to display...", buffer_.size());
    
    // Как в dis_img() - отправка данных
    wait_until_idle();
    this->write_command(0x13);
    
    this->enable();
    digitalWrite(dc_pin_, HIGH);
    
    // ТОЧНО КАК В MSP430: for(col=0; col<416; col++) for(row=0; row<30; row++)
    for (int col = 0; col < 416; col++) {
        for (int row = 0; row < 30; row++) {
            int pos = (row * 416) + col;
            if (pos < buffer_.size()) {
                // В MSP430 используется инверсия: ~gImage
                this->spi_write_byte(~buffer_[pos]);
            } else {
                this->spi_write_byte(0x00);
            }
        }
    }
    
    this->disable();
    
    // ТОЧНО КАК В MSP430: обновление
    wait_until_idle();
    this->write_command(0x04);
    wait_until_idle();
    
    wait_until_idle();
    this->write_command(0x12);
    wait_until_idle();
    
    wait_until_idle();
    this->write_command(0x02);
    wait_until_idle();
    delay(10);
    
    ESP_LOGI("weact", "Display updated");
}
	
	void wait_until_idle() {
    int timeout = 5000;
    // Ждем пока BUSY не станет HIGH (как в оригинале)
    while (digitalRead(busy_pin_) == LOW && timeout > 0) {
        delay(1);
        timeout--;
    }
    if (timeout == 0) {
        ESP_LOGW("weact", "Timeout waiting for display");
    }
}

  void enable() {
    digitalWrite(cs_pin_, LOW);
  }
  
  void disable() {
    digitalWrite(cs_pin_, HIGH);
  }
  
  void spi_write_byte(uint8_t data) {
    for (int i = 0; i < 8; i++) {
      digitalWrite(7, (data & 0x80) ? HIGH : LOW);
      data <<= 1;
      digitalWrite(6, HIGH);
      delayMicroseconds(1);
      digitalWrite(6, LOW);
      delayMicroseconds(1);
    }
  }

  int cs_pin_{0}, dc_pin_{5}, busy_pin_{3}, reset_pin_{2}, rotation_{0};
  std::vector<uint8_t> buffer_;
  std::function<void(display::DisplayBuffer &)> writer_;
};

}  // namespace weact_epaper
}  // namespace esphome
