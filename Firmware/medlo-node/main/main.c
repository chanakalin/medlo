/*
 * Medlo node
 */
#include <string.h>

#include <esp_event.h>
#include <esp_event_loop.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <lwip/sockets.h>
#include <nvs_flash.h>
#include <sdmmc_cmd.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include <cJSON.h>
#include <sdkconfig.h>
#include <hwcrypto/sha.h>
#include <mqtt_client.h>
//time & HTTPS
#include "lwip/err.h"
#include "lwip/apps/sntp.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"
#include "esp_tls.h"
#include <esp_http_client.h>
#include <time.h>
#include <sys/time.h>
#include <esp_tls.h>
//medlo
#include "medlo.h"
#include "medlo_mqtt.h"
#include "medlo_time.h"
#include "medlo_payload.h"
#include "medlo_data.h"
#include "medlo_status_led.h"
#include "medlo_storage.h"
#include "token_refresh.h"
//medlo components
#include <medlo-i2c.h>
#include <bme280.h>
#include <medlo_operator.h>
#include <si1145.h>

static esp_err_t wifi_event_handler(void *ctx, system_event_t *event) {
	switch (event->event_id) {
	case SYSTEM_EVENT_STA_START:
		esp_wifi_connect();
		break;
	case SYSTEM_EVENT_STA_GOT_IP:
		ESP_LOGI(MEDLO_TAG, "Connected to WIFI")
		;
		led_green(1);
		led_blue(0);
		ntp_init_time();
		//MQTT
		mqtt_init();
		xTaskCreate(&task_datasend, "task_datasend", 16384, NULL, 5, NULL);
		break;
	case SYSTEM_EVENT_STA_DISCONNECTED:
		//switch to offline mode
		break;
	case SYSTEM_EVENT_STA_WPS_ER_SUCCESS:
		/*point: the function esp_wifi_wps_start() only get ssid & password
		 * so call the function esp_wifi_connect() here
		 * */
		ESP_LOGI(MEDLO_TAG, "SYSTEM_EVENT_STA_WPS_ER_SUCCESS")
		;
		ESP_ERROR_CHECK(esp_wifi_wps_disable())
		;
		ESP_ERROR_CHECK(esp_wifi_connect())
		;
		ESP_LOGI(MEDLO_TAG, "Connected to WIFI using WPS")
		;
		led_green(1);
		led_blue(0);
		ntp_init_time();
		//MQTT
		//mqtt_init();
		break;
	case SYSTEM_EVENT_STA_WPS_ER_FAILED:
		ESP_LOGI(MEDLO_TAG, "SYSTEM_EVENT_STA_WPS_ER_FAILED")
		;
		ESP_ERROR_CHECK(esp_wifi_wps_disable())
		;
		ESP_ERROR_CHECK(esp_wifi_wps_enable(&wps_config))
		;
		ESP_ERROR_CHECK(esp_wifi_wps_start(0))
		;
		break;
	case SYSTEM_EVENT_STA_WPS_ER_TIMEOUT:
		ESP_LOGI(MEDLO_TAG, "SYSTEM_EVENT_STA_WPS_ER_TIMEOUT")
		;
		ESP_ERROR_CHECK(esp_wifi_wps_disable())
		;
		ESP_ERROR_CHECK(esp_wifi_wps_enable(&wps_config))
		;
		ESP_ERROR_CHECK(esp_wifi_wps_start(0))
		;
		break;
	case SYSTEM_EVENT_STA_WPS_ER_PIN:
		ESP_LOGI(MEDLO_TAG, "SYSTEM_EVENT_STA_WPS_ER_PIN")
		;
		/*show the PIN code here*/
		ESP_LOGI(MEDLO_TAG, "WPS_PIN = "PINSTR,
				PIN2STR(event->event_info.sta_er_pin.pin_code))
		;
		break;
	default:
		break;
	}
	return ESP_OK;
}

void app_main() {
	ESP_LOGI(MEDLO_TAG, "[APP] Startup..");
	ESP_LOGI(MEDLO_TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
	ESP_LOGI(MEDLO_TAG, "[APP] IDF version: %s", esp_get_idf_version());
	medlo_nvs_init();
	tcpip_adapter_init();
	medlo_gpio_init();

	medlo_spiffs_init();
	medlo_spiffs_read_tokens();
	ESP_LOGI(MEDLO_TAG,"Auth token=%s\nRefresh token=%s\n\n",authtoken,refreshtoken);


	ESP_ERROR_CHECK(esp_event_loop_init(wifi_event_handler, NULL));
	 wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT()
	 ;
	 ESP_ERROR_CHECK(esp_wifi_init(&cfg));
	 ESP_ERROR_CHECK(esp_wifi_set_storage(WIFI_STORAGE_RAM));
	 ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));

	 //WPS
	 if (wps_switch_status()) {
	 led_green(1);
	 led_blue(1);
	 ESP_ERROR_CHECK(esp_wifi_start());
	 ESP_LOGI(MEDLO_TAG, "start wps...");
	 ESP_ERROR_CHECK(esp_wifi_wps_enable(&wps_config));
	 ESP_ERROR_CHECK(esp_wifi_wps_start(0));
	 } else { //ssid and passwd
	 led_green(0);
	 led_blue(1);
	 wifi_config_t wifi_config = { .sta = { .ssid = WIFI_SSID, .password =
	 WIFI_PASSWORD, }, };
	 ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_STA, &wifi_config));
	 ESP_LOGI(MEDLO_TAG, "start the WIFI SSID:[%s]", WIFI_SSID);
	 ESP_ERROR_CHECK(esp_wifi_start());
	 ESP_LOGI(MEDLO_TAG, "Waiting for wifi");
	 }

}
