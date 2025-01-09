
#include "ina226.h"

#define SHUNT_VOLTAGE_LSB 2.5f /*uV*/
#define BUS_VOLTAGE_LSB 1.25f  /*mV*/

static inline int ina226_reg_mask_value(ina226_chip *chip, uint8_t reg,
                                        uint16_t mask, uint16_t val) {
  int ret;
  uint16_t rv = 0;
  if ((ret = chip->ops->read(chip->slave_addr, reg, (uint8_t *)&rv, 2)) != 0) {
    return ret;
  }

  rv &= ~mask;
  rv |= val;

  if ((ret = chip->ops->write(chip->slave_addr, reg, (uint8_t *)&rv, 2)) != 0) {
    return ret;
  }
  return ret;
}
int ina226_read_reg(ina226_chip *chip, uint8_t reg, uint16_t *val) {
  uint8_t buf[2];
  int ret = chip->ops->read(chip->slave_addr, reg, buf, 2);
  *val = buf[1] | (buf[0] << 8);
  return ret;
}

int ina226_write_reg(ina226_chip *chip, uint8_t reg, uint16_t val) {
  uint8_t buf[] = {val >> 8, val & 0xff};
  return chip->ops->write(chip->slave_addr, reg, buf, 2);
}

void ina226_init(ina226_chip *chip, uint8_t addr, const ina226_i2c_ops *ops) {
  chip->slave_addr = addr;
  chip->ops = ops;
}

int ina226_get_shunt_voltage(ina226_chip *chip, int16_t *vol) {
  *vol = 0;
  int ret = ina226_read_reg(chip, INA226_REG_SHUNT_VOLTAGE, (uint16_t *)vol);
  *vol = (int16_t)(SHUNT_VOLTAGE_LSB * (*vol)); // uV
  return ret;
}

int ina226_get_bus_voltage(ina226_chip *chip, int16_t *vbus) {
  *vbus = 0;
  int ret = ina226_read_reg(chip, INA226_REG_BUS_VOLTAGE, (uint16_t *)vbus);
  *vbus = (int16_t)(BUS_VOLTAGE_LSB * (*vbus));

  return ret;
}

// cur mA, shunt_res mR
void ina226_set_calibration(ina226_chip *chip, uint16_t cur,
                            uint16_t shunt_res) {
  uint16_t lsb = (uint16_t)((((uint32_t)cur) * 1000) / (1 << 15)); // uA
  if (lsb < 250) {
    chip->current_lsb = 250;
  } else if (lsb < 500) {
    chip->current_lsb = 500;
  } else if (lsb < 750) {
    chip->current_lsb = 750;
  } else if (lsb < 1000) {
    chip->current_lsb = 1000;
  } else if (lsb < 1500) {
    chip->current_lsb = 1500;
  } else {
    chip->current_lsb = (lsb % 1000 + 1) * 1000;
  }
  // cal=0.00512/(lsb(A)*shunt_res(R))
  uint16_t cal = 5120000 / (chip->current_lsb * shunt_res);
  ina226_write_reg(chip, INA226_REG_CALIBRATION, cal);
}

int ina226_get_power(ina226_chip *chip, uint16_t *power) {
  uint16_t p;
  ina226_read_reg(chip, INA226_REG_POWER, &p);

  *power = (p * (chip->current_lsb * 25)) / 1000; // mW

  return 0;
}

int ina226_get_current(ina226_chip *chip, int16_t *cur) {
  int16_t i;
  ina226_read_reg(chip, INA226_REG_CURRENT, (uint16_t *)&i);
  *cur = (i * chip->current_lsb) / 1000;
  return 0;
}
