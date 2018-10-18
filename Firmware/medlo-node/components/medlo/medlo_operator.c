#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "medlo-i2c.h"

void ac_turn_on() {
	i2c_write_byte(8, (uint8_t) 'r');
	i2c_write_byte(8, (uint8_t) 'g');
	i2c_write_byte(8, (uint8_t) 'B');
}

void ac_turn_off() {
	i2c_write_byte(8, (uint8_t) 'b');
}

void heater_turn_on() {
	i2c_write_byte(8, (uint8_t) 'g');
	i2c_write_byte(8, (uint8_t) 'b');
	i2c_write_byte(8, (uint8_t) 'R');
}

void heater_turn_off() {
	i2c_write_byte(8, (uint8_t) 'r');
}

void dehumidifier_turn_on() {
	i2c_write_byte(8, (uint8_t) 'b');
	i2c_write_byte(8, (uint8_t) 'R');
	i2c_write_byte(8, (uint8_t) 'G');
}

void dehumidifier_turn_off() {
	i2c_write_byte(8, (uint8_t) 'r');
	i2c_write_byte(8, (uint8_t) 'g');
}

void humidifier_turn_on() {
	i2c_write_byte(8, (uint8_t) 'r');
	i2c_write_byte(8, (uint8_t) 'b');
	i2c_write_byte(8, (uint8_t) 'G');
}

void humidifier_turn_off() {
	i2c_write_byte(8, (uint8_t) 'g');
}

void operator_all_off() {
	i2c_write_byte(8, (uint8_t) 'r');
	i2c_write_byte(8, (uint8_t) 'b');
	i2c_write_byte(8, (uint8_t) 'g');
}
