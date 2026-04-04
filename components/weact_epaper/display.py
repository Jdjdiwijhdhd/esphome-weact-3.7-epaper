import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import display
from esphome.const import CONF_ID, CONF_LAMBDA, CONF_CS_PIN, CONF_DC_PIN, \
    CONF_BUSY_PIN, CONF_RESET_PIN, CONF_ROTATION, CONF_UPDATE_INTERVAL

weact_epaper_ns = cg.esphome_ns.namespace("weact_epaper")
WeActEPaper = weact_epaper_ns.class_(
    "WeActEPaper", display.DisplayBuffer
)

def get_pin_number(pin_config):
    if isinstance(pin_config, int):
        return pin_config
    if hasattr(pin_config, 'number'):
        return pin_config.number
    if isinstance(pin_config, dict) and 'number' in pin_config:
        return pin_config['number']
    return 0

CONFIG_SCHEMA = display.FULL_DISPLAY_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(WeActEPaper),
    cv.Required(CONF_CS_PIN): cv.int_,
    cv.Required(CONF_DC_PIN): cv.int_,
    cv.Required(CONF_BUSY_PIN): cv.int_,
    cv.Required(CONF_RESET_PIN): cv.int_,
    cv.Optional(CONF_ROTATION, default=0): cv.int_range(min=0, max=3),
    cv.Optional(CONF_UPDATE_INTERVAL, default="30s"): cv.update_interval,
})

async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    
    cs_pin = get_pin_number(config[CONF_CS_PIN])
    dc_pin = get_pin_number(config[CONF_DC_PIN])
    busy_pin = get_pin_number(config[CONF_BUSY_PIN])
    reset_pin = get_pin_number(config[CONF_RESET_PIN])
    
    cg.add(var.set_cs_pin(cs_pin))
    cg.add(var.set_dc_pin(dc_pin))
    cg.add(var.set_busy_pin(busy_pin))
    cg.add(var.set_reset_pin(reset_pin))
    cg.add(var.set_rotation(config[CONF_ROTATION]))
    cg.add(var.set_buffer_size(416 * 240 // 8))
    
    if CONF_LAMBDA in config:
        # Используем ссылку на DisplayBuffer через &it
        lambda_ = await cg.process_lambda(
            config[CONF_LAMBDA], [(display.DisplayBuffer, "&it")], return_type=cg.void
        )
        cg.add(var.set_writer(lambda_))
