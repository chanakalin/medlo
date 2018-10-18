#include <driver/i2c.h>

#define I2C_MASTER_SCL_IO          26
#define I2C_MASTER_SDA_IO          25
#define I2C_MASTER_NUM             I2C_NUM_0
#define I2C_MASTER_TX_BUF_DISABLE  0
#define I2C_MASTER_RX_BUF_DISABLE  0
#define I2C_MASTER_FREQ_HZ         100000

#define WRITE_BIT                          I2C_MASTER_WRITE
#define READ_BIT                           I2C_MASTER_READ
#define ACK_CHECK_EN                       0x1
#define ACK_CHECK_DIS                      0x0
#define ACK_VAL                            0x0
#define NACK_VAL                           0x1


esp_err_t i2c_init();
esp_err_t i2c_write_byte(uint8_t address, uint8_t data);
esp_err_t i2c_write_register(uint8_t address, uint8_t reg_address, uint8_t data);
esp_err_t i2c_read_byte(uint8_t address, uint8_t* data);
esp_err_t i2c_write_bytes(uint8_t address, uint8_t* dataArray,int size);
esp_err_t i2c_read_bytes(uint8_t address, uint8_t* dataArray, int size) ;
//
ushort i2c_read_ushort(uint8_t address, uint8_t reg);
short i2c_read_short(uint8_t address, uint8_t reg);
uint8_t i2c_read_uint8(uint8_t address, uint8_t reg);
int8_t i2c_read_int8(uint8_t address, uint8_t reg);
uint16_t i2c_read_uint16(uint8_t address, uint8_t reg);
