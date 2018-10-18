#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <stdio.h>
#include <driver/i2c.h>

#include <medlo-i2c.h>
#include <si1145.h>


//init SI1145
esp_err_t si1145_init() {
	int ret;
	uint8_t partid, status;
	ret = i2c_write_byte(SI1145_ADDRESS, SI1145_REG_PARTID);
	if (ret != ESP_OK) {
		return ret;
	}
	ret = i2c_read_byte(SI1145_ADDRESS, &partid);
	ESP_LOGI(SI1145_TAG, "part id = %x", partid);
	if(partid==0b01000101){//page 31 datasheet
		ESP_LOGI(SI1145_TAG, "part id successfully returned");
		//write further configurations
		// enable UVindex measurement coefficients!
		i2c_write_register(SI1145_ADDRESS,SI1145_REG_UCOEFF0, 0x29);
		i2c_write_register(SI1145_ADDRESS,SI1145_REG_UCOEFF1, 0x89);
		i2c_write_register(SI1145_ADDRESS,SI1145_REG_UCOEFF2, 0x02);
		i2c_write_register(SI1145_ADDRESS,SI1145_REG_UCOEFF3, 0x00);
		// enable UV sensor
		si1145_write_param(SI1145_PARAM_CHLIST,
				SI1145_PARAM_CHLIST_ENUV | SI1145_PARAM_CHLIST_ENALSIR
						| SI1145_PARAM_CHLIST_ENALSVIS
						| SI1145_PARAM_CHLIST_ENPS1);
		// enable interrupt on every sample
		i2c_write_register(SI1145_ADDRESS, SI1145_REG_INTCFG,
				SI1145_REG_INTCFG_INTOE);
		i2c_write_register(SI1145_ADDRESS, SI1145_REG_IRQEN,
				SI1145_REG_IRQEN_ALSEVERYSAMPLE);
		// program LED current
		i2c_write_register(SI1145_ADDRESS, SI1145_REG_PSLED21, 0x03); // 20mA for LED 1 only
		si1145_write_param(SI1145_PARAM_PS1ADCMUX, SI1145_PARAM_ADCMUX_LARGEIR);
		// prox sensor #1 uses LED #1
		si1145_write_param(SI1145_PARAM_PSLED12SEL,
				SI1145_PARAM_PSLED12SEL_PS1LED1);
		// fastest clocks, clock div 1
		si1145_write_param(SI1145_PARAM_PSADCGAIN, 0);
		// take 511 clocks to measure
		si1145_write_param(SI1145_PARAM_PSADCOUNTER,
				SI1145_PARAM_ADCCOUNTER_511CLK);
		// in prox mode, high range
		si1145_write_param(SI1145_PARAM_PSADCMISC,
				SI1145_PARAM_PSADCMISC_RANGE | SI1145_PARAM_PSADCMISC_PSMODE);

		si1145_write_param(SI1145_PARAM_ALSIRADCMUX,
				SI1145_PARAM_ADCMUX_SMALLIR);
		// fastest clocks, clock div 1
		si1145_write_param(SI1145_PARAM_ALSIRADCGAIN, 0);
		// take 511 clocks to measure
		si1145_write_param(SI1145_PARAM_ALSIRADCOUNTER,
				SI1145_PARAM_ADCCOUNTER_511CLK);
		// in high range mode
		si1145_write_param(SI1145_PARAM_ALSIRADCMISC,
				SI1145_PARAM_ALSIRADCMISC_RANGE);
		// fastest clocks, clock div 1
		si1145_write_param(SI1145_PARAM_ALSVISADCGAIN, 0);
		// take 511 clocks to measure
		si1145_write_param(SI1145_PARAM_ALSVISADCOUNTER,
				SI1145_PARAM_ADCCOUNTER_511CLK);
		// in high range mode (not normal signal)
		si1145_write_param(SI1145_PARAM_ALSVISADCMISC,
				SI1145_PARAM_ALSVISADCMISC_VISRANGE);
		/************************/
		// measurement rate for auto
		i2c_write_register(SI1145_ADDRESS,SI1145_REG_MEASRATE0, 0xFF); // 255 * 31.25uS = 8ms
		// auto run
		ret = i2c_write_register(SI1145_ADDRESS,SI1145_REG_COMMAND, SI1145_PSALS_AUTO);
	}

	return ret;
}

esp_err_t si1145_reset() {
	esp_err_t ret;
	i2c_write_register(SI1145_ADDRESS,SI1145_REG_MEASRATE0, 0);
	i2c_write_register(SI1145_ADDRESS,SI1145_REG_MEASRATE1, 0);
	i2c_write_register(SI1145_ADDRESS,SI1145_REG_IRQEN, 0);
	i2c_write_register(SI1145_ADDRESS,SI1145_REG_IRQMODE1, 0);
	i2c_write_register(SI1145_ADDRESS,SI1145_REG_IRQMODE2, 0);
	i2c_write_register(SI1145_ADDRESS,SI1145_REG_INTCFG, 0);
	i2c_write_register(SI1145_ADDRESS,SI1145_REG_IRQSTAT, 0xFF);

	i2c_write_register(SI1145_ADDRESS,SI1145_REG_COMMAND, SI1145_RESET);
	vTaskDelay(10 / portTICK_PERIOD_MS);
	ret= i2c_write_register(SI1145_ADDRESS,SI1145_REG_HWKEY, 0x17);
	vTaskDelay(10 / portTICK_PERIOD_MS);
	return ret;
}

//write SI1145 parameter
esp_err_t si1145_write_param(uint8_t p, uint8_t v) {
	int paramd;
	i2c_write_register(SI1145_ADDRESS,SI1145_REG_PARAMWR, v);
	i2c_write_register(SI1145_ADDRESS,SI1145_REG_COMMAND, p | SI1145_PARAM_SET);
	i2c_write_byte(SI1145_ADDRESS, SI1145_REG_PARAMRD);
	return i2c_read_byte(SI1145_ADDRESS, &paramd);
}

//read UV
uint16_t si1145_read_UV(){
	return i2c_read_uint16(SI1145_ADDRESS,0x2C);
}

//read visible (visible+IR level)
uint16_t si1145_read_visible(){
	return i2c_read_uint16(SI1145_ADDRESS,0x22);
}

//read IR
uint16_t si1145_read_IR(){
	return i2c_read_uint16(SI1145_ADDRESS,0x24);
}

//read proximity (assumes an IR LED is attached to LED)
uint16_t si1145_read_proximity(){
	return i2c_read_uint16(SI1145_ADDRESS,0x26);
}
