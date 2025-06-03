// I2C dosyanını header dosyası, fonskiyonları prototipleri var
#ifndef I2C_H_
#define I2C_H_

void i2c_init();
bool i2c_write_byte(uint8_t byte);
bool i2c_read_addr8_data8(uint8_t addr, uint8_t *data);
bool i2c_write_addr8_data8(uint8_t addr, uint8_t data);
uint16_t i2c_read_word(uint8_t addr);

#endif /* I2C_H_ */
