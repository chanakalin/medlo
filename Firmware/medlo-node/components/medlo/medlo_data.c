#include <esp_event.h>
#include <esp_event_loop.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "medlo-i2c.h"
#include "bme280.h"
#include "si1145.h"
#include "medlo_mqtt.h"
#include "medlo_payload.h"
#include "medlo_data.h"
#include "medlo_status_led.h"
#include "medlo_storage.h"
#include "medlo_operator.h"


uint8_t datasend_sensors;

void datasend_init() {
	ESP_LOGI(MEDLO_DATA_TAG,"Initializing sensors...");
	i2c_init();
	datasend_sensors = 0;
	if (bme280_init() == ESP_OK) {
		datasend_sensors |= (1 << SENSOR_BME280); //sensor available
		ESP_LOGI(MEDLO_DATA_TAG,"BME280 found");
	}
	if (si1145_init() == ESP_OK) {
		datasend_sensors |= (1 << SENSOR_SI1145); //sensor available
		ESP_LOGI(MEDLO_DATA_TAG,"SI1145 found");
	}
	//parameters from NVS
	nvs_get_i16(medlo_nvs_handle,ADDR_NVS_TEMP_MAX,&medlo_param_temp_max);
	nvs_get_i16(medlo_nvs_handle,ADDR_NVS_TEMP_MIN,&medlo_param_temp_min);
	nvs_get_i16(medlo_nvs_handle,ADDR_NVS_HUMID_MAX,&medlo_param_humid_max);
	nvs_get_i16(medlo_nvs_handle,ADDR_NVS_HUMID_MIN,&medlo_param_humid_min);
}

void task_datasend( void *pvParameters) {
	datasend_init();//init
	while (1) {
		led_blue(1);//indicate
		if (datasend_sensors & (1 << SENSOR_BME280)) { //if bme280 available
			bme280_reading_data bme280Data = bme280_read_values();
			payload_send_sensor("temperature", bme280Data.temperature); //temperature
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			payload_send_sensor("humidity", bme280Data.humidity); //humidity
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			//operator
			if((int16_t)bme280Data.temperature>medlo_param_temp_max){//above max temp
				heater_turn_off();
				humidifier_turn_off();
				dehumidifier_turn_off();
				ac_turn_on();
				ESP_LOGW(MEDLO_DATA_TAG,"Turning on AC until %d c (value=%d)",medlo_param_temp_max,(int16_t)bme280Data.temperature);
			}else if((int16_t)bme280Data.temperature<medlo_param_temp_min){//below min temp
				ac_turn_off();
				humidifier_turn_off();
				dehumidifier_turn_off();
				heater_turn_on();
				ESP_LOGW(MEDLO_DATA_TAG,"Turning on Heater until %d c (value=%d)",medlo_param_temp_min,(int16_t)bme280Data.temperature);
			}else if((int16_t)bme280Data.humidity>medlo_param_humid_max){//above max humid
				heater_turn_off();
				ac_turn_off();
				humidifier_turn_off();
				dehumidifier_turn_on();
				ESP_LOGW(MEDLO_DATA_TAG,"Turning on Dehumidifier until %d",medlo_param_humid_max);
			}else if((int16_t)bme280Data.humidity<medlo_param_humid_min){//below min humid
				heater_turn_off();
				ac_turn_off();
				dehumidifier_turn_off();
				humidifier_turn_on();
				ESP_LOGW(MEDLO_DATA_TAG,"Turning on Humidifier until %d",medlo_param_humid_min);
			}else{//operator all off
				heater_turn_off();
				ac_turn_off();
				dehumidifier_turn_off();
				humidifier_turn_off();
			}
			//operator end
		}
		if (datasend_sensors & (1 << SENSOR_SI1145)) { //if bme280 available
			payload_send_sensor("uvindex", si1145_read_UV()); //uvindex
			vTaskDelay(1000 / portTICK_PERIOD_MS);
			payload_send_sensor("visiblelight", si1145_read_visible()); //visiblelight
			vTaskDelay(1000 / portTICK_PERIOD_MS);
		}
		led_blue(0);//indicate off
		vTaskDelay((MEDLO_PUSH_INTERVAL*1000) / portTICK_PERIOD_MS);
	}
}
