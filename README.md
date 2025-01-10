该用于控制TI的ina226芯片，设置以及电压电流读取等。

### 使用
#### 添加库
如果使用cmake可直接把库clone到项目目录下，比如libs/ina226,然后项目在CMakeLists.txt增加：add_subdirectory(libs/ina226)，之后链接上ina226静态库：target_link_libraries(my_app ina226).
其他的可以复制ina226.c和ina226.h进项目。
#### 代码实现
项目依赖添加成功后，代码中只需要实现struct ina226_i2c_ops 结构体中i2c读写函数指针，再调用ina226_init把结构体传递进去就可完成初始化，后续可以调用ina226相关函数。下面是stm32 hal相关例子：
```c
#define INA226_ADDR 0x40 << 1

extern I2C_HandleTypeDef hi2c1;

static int8_t i2c_read(uint8_t salve_addr, uint8_t reg, uint8_t *buf,
                       uint16_t len) {
  return HAL_I2C_Mem_Read(&hi2c1, salve_addr, reg, I2C_ADDRESSINGMODE_7BIT, buf,
                          len, 10);
}

static int8_t i2c_write(uint8_t salve_addr, uint8_t reg, const uint8_t *buf,
                        uint16_t len) {
  return HAL_I2C_Mem_Write(&hi2c1, salve_addr, reg, I2C_ADDRESSINGMODE_7BIT,
                           (uint8_t *)buf, len, 10);
}
static const ina226_i2c_ops ops = {
    .read = i2c_read,
    .write = i2c_write,
};

ina226_chip ina226; 
ina226_init(&ina226, INA226_ADDR, &ops); //完成初始化
ina226_set_calibration(&ina226,8000,5);  //设置电流最大量程为8A,采样电阻5毫欧

```
