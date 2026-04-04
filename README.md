# ESPHome Component for WeAct Studio 3.7" E-Paper Display


Поддержка чб дисплея WeAct Studio 3.7" 416x240 с контроллером UC8253. При тестах использовалась плата ESP32-C3 Supermini.

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
    rotation: 0 # повороты 1 - 90 градусов, 2 - 180, 3 - 270
    update_interval: 30s # периоды обновления дисплея
```
## Результат
![PXL_20260404_203116256](https://github.com/user-attachments/assets/99e78bee-bd3f-4f87-871c-40998a0f6b8a)

Ссылка на репозиторий WeAct https://github.com/WeActStudio/WeActStudio.EpaperModule
