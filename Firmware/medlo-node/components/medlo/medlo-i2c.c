#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <stdio.h>
#include <driver/i2c.h>

#include <medlo-i2c.h>

/**init I2C master
 * SDA = 18
 * SCL = 19
 * Enable internal pullup resisters
 **/
esp_err_t i2c_init() {
	int i2c_master_port = I2C_MASTER_NUM;
	i2c_config_t conf;
	conf.mode = I2C_MODE_MASTER;
	conf.sda_io_num = I2C_MASTER_SDA_IO;
	conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
	conf.scl_io_num = I2C_MASTER_SCL_IO;
	conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
	conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
	i2c_param_config(i2c_master_port, &conf);
	return i2c_driver_install(i2c_master_port, conf.mode,
	I2C_MASTER_RX_BUF_DISABLE,
	I2C_MASTER_TX_BUF_DISABLE, 0);
}

//write a byte
esp_err_t i2c_write_byte(uint8_t address, uint8_t data) {
	int ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, address << 1 | WRITE_BIT,
	ACK_CHECK_EN);
	i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

//write to a register
esp_err_t i2c_write_register(uint8_t address, uint8_t reg_address, uint8_t data) {
	int ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, address << 1 | WRITE_BIT,
	ACK_CHECK_EN);
	i2c_master_write_byte(cmd, reg_address, ACK_CHECK_EN);
	i2c_master_write_byte(cmd, data, ACK_CHECK_EN);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

//read a byte
esp_err_t i2c_read_byte(uint8_t address, uint8_t* data) {
	int ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, address << 1 | READ_BIT,
	ACK_CHECK_EN);
	i2c_master_read_byte(cmd, data, NACK_VAL);
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

//write an array
esp_err_t i2c_write_bytes(uint8_t address, uint8_t* dataArray, int size) {
	int ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, address << 1 | WRITE_BIT,
	ACK_CHECK_EN);
	for (int i = 0; i < size; i++) {
		i2c_master_write_byte(cmd, dataArray, ACK_CHECK_EN);
		dataArray++;
	}
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

//read an array
esp_err_t i2c_read_bytes(uint8_t address, uint8_t* dataArray, int size) {
	int ret;
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, address << 1 | READ_BIT,
	ACK_CHECK_EN);
	for (int i = 0; i < size; i++) {
		if ((i + 1) == size) { //no ACK for the last
			i2c_master_read_byte(cmd, dataArray, NACK_VAL);
		} else {
			i2c_master_read_byte(cmd, dataArray, ACK_VAL);
		}
		dataArray++;
	}
	i2c_master_stop(cmd);
	ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return ret;
}

//read unsigned short
ushort i2c_read_ushort(uint8_t address, uint8_t reg) {
	uint8_t data_l, data_h;
	i2c_write_byte(address, reg); //write register 1
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, address << 1 | READ_BIT,
	ACK_CHECK_EN);
	i2c_master_read_byte(cmd, &data_l, ACK_VAL);
	i2c_master_read_byte(cmd, &data_h, NACK_VAL);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return (ushort) ((data_h << 8) | data_l);
}

//read signed short
short i2c_read_short(uint8_t address, uint8_t reg) {
	uint8_t data_l, data_h;
	i2c_write_byte(address, reg); //write register 1
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, address << 1 | READ_BIT,
	ACK_CHECK_EN);
	i2c_master_read_byte(cmd, &data_l, ACK_VAL);
	i2c_master_read_byte(cmd, &data_h, NACK_VAL);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return (short) ((data_h << 8) | data_l);
}

//read unsigned char
uint8_t i2c_read_uint8(uint8_t address, uint8_t reg) {
	uint8_t data;
	i2c_write_byte(address, reg); //write register 1
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, address << 1 | READ_BIT,
	ACK_CHECK_EN);
	i2c_master_read_byte(cmd, &data, NACK_VAL);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return data;
}

//read sined int8
int8_t i2c_read_int8(uint8_t address, uint8_t reg) {
	uint8_t data;
	i2c_write_byte(address, reg); //write register 1
	i2c_cmd_handle_t cmd = i2c_cmd_link_create();
	i2c_master_start(cmd);
	i2c_master_write_byte(cmd, address << 1 | READ_BIT,
	ACK_CHECK_EN);
	i2c_master_read_byte(cmd, &data, NACK_VAL);
	i2c_master_stop(cmd);
	i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_RATE_MS);
	i2c_cmd_link_delete(cmd);
	return (int8_t) data;
}

//read unsigned int16
uint16_t i2c_read_uint16(uint8_t address, uint8_t reg) {
	i2c_write_byte(address, reg);
	uint8_t bufferArray[2];
	uint8_t *buffer = &bufferArray;
	i2c_read_bytes(address, buffer, sizeof(bufferArray));
	return bufferArray[1] << 8 | bufferArray[0];
}
