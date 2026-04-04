import esphome.codegen as cg
from esphome.components import display
from esphome.cpp_helpers import gpio_pin_expression
from esphome.const import CONF_CS_PIN, CONF_DC_PIN, CONF_BUSY_PIN, CONF_RESET_PIN

weact_epaper_ns = cg.esphome_ns.namespace("weact_epaper")
WeActEPaper = weact_epaper_ns.class_("WeActEPaper", display.DisplayBuffer, cg.Component)


async def register_weact_epaper(var, config):
    """Register the weact epaper display."""
    await display.register_display(var, config)

    # Setup pins
    cs_pin = await gpio_pin_expression(config[CONF_CS_PIN])
    cg.add(var.set_cs_pin(cs_pin))
    dc_pin = await gpio_pin_expression(config[CONF_DC_PIN])
    cg.add(var.set_dc_pin(dc_pin))
    busy_pin = await gpio_pin_expression(config[CONF_BUSY_PIN])
    cg.add(var.set_busy_pin(busy_pin))
    reset_pin = await gpio_pin_expression(config[CONF_RESET_PIN])
    cg.add(var.set_reset_pin(reset_pin))