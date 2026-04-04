# ESPHome Component for WeAct Studio 3.7" E-Paper Display

Поддержка дисплея WeAct Studio 3.7" 416x240 с контроллером UC8253.

## Подключение

| Display | ESP32 |
|---------|-------|
| CS      | GPIO0 |
| DC      | GPIO5 |
| BUSY    | GPIO3 |
| RESET   | GPIO2 |
| CLK     | GPIO6 |
| MOSI    | GPIO7 |
| VCC     | 3.3V/5V |
| GND     | GND |

## Использование

```yaml
external_components:
  - source: github://Jdjdiwijhdhd/esphome-weact-3.7-epaper

display:
  - platform: weact_epaper
    cs_pin: 0
    dc_pin: 5
    busy_pin: 3
    reset_pin: 2
    rotation: 90
    update_interval: 30s