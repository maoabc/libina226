#ifndef __INA226_H
#define __INA226_H

#include <stdint.h>

typedef enum {
    INA226_REG_CONFIGURATION = 0x00,
    INA226_REG_SHUNT_VOLTAGE = 0x01,
    INA226_REG_BUS_VOLTAGE = 0x02,
    INA226_REG_POWER = 0x03,
    INA226_REG_CURRENT = 0x04,
    INA226_REG_CALIBRATION = 0x05,
    INA226_REG_MASK = 0x06,
    INA226_REG_ALERT_LIMIT = 0x07,
    INA226_REG_MANUFACTURER_ID = 0xFE,
    INA226_REG_DIE_ID = 0xFF,
} ina266_reg_e;

#define INA226_CONF_MODE_MASK 0x7
#define INA226_CONF_VSHCT_MASK 0x7 << 3
#define INA226_CONF_VBUSCT_MASK 0x7 << 6
#define INA226_CONF_AVG_MASK 0x7 << 9
#define INA226_CONF_RST_MASK 0x01 << 15

#define INA226_MAKE_CONF_REG(_mode, _vshct, _vbusct, _avg)                     \
  (((_mode) & 0x7) | (((_vshct) & 0x7) << 3) | (((_vbusct) & 0x7) << 6) |      \
   (((_avg) & 0x7) << 9) | (0x04 << 12) | (((_rst) & 0x1) << 15))

#define INA226_GET_CONF_MODE(_c) ((_c) & INA226_CONF_MODE_MASK)
#define INA226_GET_CONF_VSHCT(_c) (((_c) & INA226_CONF_VSHCT_MASK) >> 3)
#define INA226_GET_CONF_VBUSCT(_c) (((_c) & INA226_CONF_VBUSCT_MASK) >> 6)
#define INA226_GET_CONF_AVG(_c) (((_c) & INA226_CONF_AVG_MASK) >> 9)
#define INA226_GET_CONF_RST(_c) (((_c) & INA226_CONF_RST_MASK) >> 15)

typedef struct ina226_i2c_ops {
    int8_t (*read)(uint8_t salve_addr, uint8_t reg, uint8_t *buf, uint16_t len);

    int8_t (*write)(uint8_t salve_addr, uint8_t reg, const uint8_t *buf,
                    uint16_t len);
} ina226_i2c_ops;

typedef struct ina226_chip {
    const ina226_i2c_ops *ops;
    uint8_t slave_addr;
    uint16_t current_lsb;//uA
} ina226_chip;

void ina226_init(ina226_chip *chip, uint8_t addr, const ina226_i2c_ops *ops);

int ina226_read_reg(ina226_chip *chip, uint8_t reg, uint16_t *val);

int ina226_write_reg(ina226_chip *chip, uint8_t reg, uint16_t val);


void ina226_set_calibration(ina226_chip *chip, uint16_t cur,
                            uint16_t shunt_res);

int ina226_get_shunt_voltage(ina226_chip *chip, int16_t *vol);

int ina226_get_bus_voltage(ina226_chip *chip, int16_t *vbus);

int ina226_get_power(ina226_chip *chip, uint16_t *power);

int ina226_get_current(ina226_chip *chip, int16_t *cur);

#endif
