#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include "medlo_status_led.h"

void medlo_gpio_init(){
	 //GPIO
	 gpio_set_direction(LED_GREEN, GPIO_MODE_OUTPUT);
	 gpio_set_direction(LED_BLUE, GPIO_MODE_OUTPUT);
	 gpio_set_direction(WPS_SWITCH,GPIO_MODE_INPUT);
	 gpio_pullup_en(WPS_SWITCH);
	 /**gpio_config_t wps_switch_config = { .pin_bit_mask = WPS_SWITCH, .mode =
	 GPIO_MODE_INPUT, .pull_up_en = GPIO_PULLUP_ENABLE, .pull_down_en =
	 GPIO_PULLDOWN_DISABLE, .intr_type = GPIO_INTR_DISABLE, };
	 gpio_config(&wps_switch_config);**/
	 //GPIO END
}

//gpio
void led_green(uint8_t status) {
	if (status) {
		gpio_set_level(LED_GREEN, 1);
	} else {
		gpio_set_level(LED_GREEN, 0);
	}
}
void led_blue(uint8_t status) {
	if (status) {
		gpio_set_level(LED_BLUE, 1);
	} else {
		gpio_set_level(LED_BLUE, 0);
	}
}


uint8_t wps_switch_status(){
	if(gpio_get_level(WPS_SWITCH)){
		return false;//not pressed
	}else{
		return true;//pressed
	}
}
