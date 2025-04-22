import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.components import climate
from esphome.const import CONF_ID

AUTO_LOAD = ["climate"]

fujitsu_264_ns = cg.esphome_ns.namespace('fujitsu_264')
Fujitsu264Climate = fujitsu_264_ns.class_('Fujitsu264Climate', climate.Climate)

CONFIG_SCHEMA = climate.CLIMATE_SCHEMA.extend({
    cv.GenerateID(): cv.declare_id(Fujitsu264Climate),
})


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await climate.register_climate(var, config)
    cg.add_library(
        name="IRremoteESP8266",
        repository="https://github.com/shimmy-void/IRremoteESP8266.git#bce5c0b0eeda8431a95d95c9947e4008bb0b982f",
        version="bce5c0b0eeda8431a95d95c9947e4008bb0b982f",
    )
