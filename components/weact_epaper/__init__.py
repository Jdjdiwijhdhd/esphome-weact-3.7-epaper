import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display, spi
from esphome.const import CONF_ID, CONF_CS_PIN, CONF_DC_PIN, CONF_BUSY_PIN, CONF_RESET_PIN

DEPENDENCIES = ["spi"]

weact_epaper_ns = cg.esphome_ns.namespace("weact_epaper")
WeActEPaper = weact_epaper_ns.class_("WeActEPaper", display.DisplayBuffer, spi.SPIDevice)

CONFIG_SCHEMA = display.FULL_DISPLAY_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(WeActEPaper),
    cv.Required(CONF_CS_PIN): cv.int_,
    cv.Required(CONF_DC_PIN): cv.int_,
    cv.Required(CONF_BUSY_PIN): cv.int_,
    cv.Required(CONF_RESET_PIN): cv.int_,
}).extend(spi.spi_device_schema())


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await display.register_display(var, config)
    await spi.register_spi_device(var, config)

    cg.add(var.set_cs_pin(config[CONF_CS_PIN]))
    cg.add(var.set_dc_pin(config[CONF_DC_PIN]))
    cg.add(var.set_busy_pin(config[CONF_BUSY_PIN]))
    cg.add(var.set_reset_pin(config[CONF_RESET_PIN]))