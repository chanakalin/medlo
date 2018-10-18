#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <stdio.h>
#include <driver/i2c.h>

#include <medlo-i2c.h>
#include <bme280.h>

esp_err_t bme280_init() {
	int ret;
	uint8_t chipid, status;
	ret = i2c_write_byte(BME280_ADDRESS, BME280_REGISTER_CHIPID);
	if (ret != ESP_OK) {
		return ret;
	}
	ret = i2c_read_byte(BME280_ADDRESS, &chipid);

	ESP_LOGI(BME280_TAG, "chip id = %x", chipid);
	if (chipid == 0x60) { //got correct chip id
		//power cycle reset
		i2c_write_register(BME280_ADDRESS, BME280_REGISTER_RESET,
				BME280_COMMAND_RESET);
		//read status and wait if in reading calibrations
		i2c_write_byte(BME280_ADDRESS, BME280_REGISTER_STATUS);
		vTaskDelay(100 / portTICK_RATE_MS);		//wait for reset
		i2c_read_byte(BME280_ADDRESS, &status);
		while (status & (1 << 0)) {
			ESP_LOGW(BME280_TAG, "Waiting for calibrations in reading");
			vTaskDelay(100 / portTICK_RATE_MS);
		}
		//read calibration data (table 16, page 22 of datasheet)
		bme280_calib.dig_T1 = i2c_read_ushort(BME280_ADDRESS,
				BME280_REGISTER_DIG_T1);
		bme280_calib.dig_T2 = i2c_read_short(BME280_ADDRESS,
				BME280_REGISTER_DIG_T2);
		bme280_calib.dig_T3 = i2c_read_short(BME280_ADDRESS,
				BME280_REGISTER_DIG_T3);
		bme280_calib.dig_H1 = i2c_read_uint8(BME280_ADDRESS,
				BME280_REGISTER_DIG_H1);
		bme280_calib.dig_H2 = i2c_read_short(BME280_ADDRESS,
				BME280_REGISTER_DIG_H2);
		bme280_calib.dig_H3 = i2c_read_uint8(BME280_ADDRESS,
				BME280_REGISTER_DIG_H3);
		bme280_calib.dig_H4 = (short) ((i2c_read_uint8(BME280_ADDRESS,
				BME280_REGISTER_DIG_H4) << 4)
				| ((i2c_read_uint8(BME280_ADDRESS, BME280_REGISTER_DIG_H4 + 1)
						& 0xF)));
		bme280_calib.dig_H5 =
				(short) ((i2c_read_uint8(BME280_ADDRESS,
						BME280_REGISTER_DIG_H5 + 1) << 4)
						| (i2c_read_uint8(BME280_ADDRESS,
								BME280_REGISTER_DIG_H5) >> 4));
		bme280_calib.dig_H6 = i2c_read_int8(BME280_ADDRESS,
				BME280_REGISTER_DIG_H6);
		ESP_LOGI(BME280_TAG, "Calibration readings fine");
		//
		//set configurations
		i2c_write_register(BME280_ADDRESS, BME280_REGISTER_CONTROL, 0b00000000);//send to sleep for writing configs (page 27 datasheet)
		i2c_write_register(BME280_ADDRESS, BME280_REGISTER_CONFIG, 0b01110100);	//normal mode inctive time 250ms,IIR filter cof = 16 (page 28 datasheet)
		i2c_write_register(BME280_ADDRESS, BME280_REGISTER_CONTROLHUMID,
				0b00000101);	//humidity oversampling X 0 (page 26 datasheet)
		i2c_write_register(BME280_ADDRESS, BME280_REGISTER_CONTROL, 0b10100011);//temperature oversampling X 16,no presuure, normal mode (page 27 datasheet)
		//
		ESP_LOGI(BME280_TAG, "Configuration setup fine");
	} else {		//invalid chip id
		ESP_LOGE(BME280_TAG,"BAD Chip ID %X",chipid);
		return ESP_ERR_INVALID_RESPONSE;
	}

	return ret;
}

//burst read
bme280_reading_data bme280_read_values() {
	bme280_reading_data ret_data;
	uint8_t status;
	//
	status = (1 << 3);		//measuring
	while (status & (1 << 3)) {		//wait for measuring
		status = i2c_read_uint8(BME280_ADDRESS, BME280_REGISTER_STATUS);
		//ESP_LOGW(BME280_TAG, "Waiting for measuring");
		vTaskDelay(100 / portTICK_RATE_MS);
	}
	//burst reading
	i2c_write_byte(BME280_ADDRESS, BME280_REGISTER_PRESSUREDATA);
	uint8_t bufferArray[8];
	uint8_t *buffer = &bufferArray;
	i2c_read_bytes(BME280_ADDRESS, buffer, sizeof(bufferArray));
	//

	bme280_adc adc;
	adc.adc_T = (BME280_S32_t) (bufferArray[3] << 12 | bufferArray[4] << 4
			| bufferArray[5] >> 4);
	//ESP_LOGI(BME280_TAG, "temp_m=%X temp_l=%X temp_xl=%X", bufferArray[3],
	//		bufferArray[4], bufferArray[5]);
	adc.adc_H = (BME280_S32_t) (bufferArray[6] << 8 | bufferArray[7]);
	//ESP_LOGI(BME280_TAG, "humid_m=%X humid_l=%X", bufferArray[6],
	//		bufferArray[7]);

	//
	//calculate temperature
	ret_data.temperature = bme280_calculate_temperature(adc.adc_T);
	//calculate humidity
	ret_data.humidity = bme280_calculate_humidity(adc.adc_H);

	return ret_data;
}

float bme280_calculate_temperature(BME280_S32_t adc_T) { //Page 23 datasheet
	BME280_S32_t var1, var2, T;

	var1 = ((((adc_T >> 3) - ((BME280_S32_t) bme280_calib.dig_T1 << 1)))
			* ((BME280_S32_t) bme280_calib.dig_T2)) >> 11;
	var2 = (((((adc_T >> 4) - ((BME280_S32_t) bme280_calib.dig_T1))
			* ((adc_T >> 4) - ((BME280_S32_t) bme280_calib.dig_T1))) >> 12)
			* ((BME280_S32_t) bme280_calib.dig_T3)) >> 14;
	t_fine = var1 + var2;
	T = (t_fine * 5 + 128) >> 8;
	return (float) T / 100.0;
}

float bme280_calculate_humidity(BME280_S32_t adc_H) { //Page 24 datasheet
	BME280_S32_t v_x1_u32r, h;
	v_x1_u32r = (t_fine - ((BME280_S32_t) 76800));
	v_x1_u32r =
			(((((adc_H << 14) - (((BME280_S32_t) bme280_calib.dig_H4) << 20)
					- (((BME280_S32_t) bme280_calib.dig_H5) * v_x1_u32r))
					+ ((BME280_S32_t) 16384)) >> 15)
					* (((((((v_x1_u32r * ((BME280_S32_t) bme280_calib.dig_H6))
							>> 10)
							* (((v_x1_u32r
									* ((BME280_S32_t) bme280_calib.dig_H3))
									>> 11) + ((BME280_S32_t) 32768))) >> 10)
							+ ((BME280_S32_t) 2097152))
							* ((BME280_S32_t) bme280_calib.dig_H2) + 8192) >> 14));
	v_x1_u32r = (v_x1_u32r
			- (((((v_x1_u32r >> 15) * (v_x1_u32r >> 15)) >> 7)
					* ((BME280_S32_t) bme280_calib.dig_H1)) >> 4));
	v_x1_u32r = (v_x1_u32r < 0 ? 0 : v_x1_u32r);
	v_x1_u32r = (v_x1_u32r > 419430400 ? 419430400 : v_x1_u32r);
	h = (BME280_U32_t) (v_x1_u32r >> 12);
	return (float) h / 1024.0;
}
