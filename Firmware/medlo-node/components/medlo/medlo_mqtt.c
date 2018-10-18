#include <esp_system.h>
#include <esp_err.h>
#include <esp_log.h>
#include <cJSON.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "mqtt_client.h"
#include "medlo_mqtt.h"
#include "medlo_data.h"
#include "medlo_storage.h"
#include "medlo_status_led.h"

static esp_err_t mqtt_event_handler(esp_mqtt_event_handle_t event) {
	esp_mqtt_client_handle_t client = event->client;
	int msg_id = 0;
	// your_context_t *context = event->context;
	switch (event->event_id) {
	case MQTT_EVENT_CONNECTED:
		ESP_LOGI(MQTT_TAG, "MQTT_EVENT_CONNECTED")
		;
		sprintf(topic, "carbon.super/medlonode/%s/configranges",
		MEDLO_DEVICE_ID);
		msg_id = esp_mqtt_client_subscribe(client, topic, 0);
		ESP_LOGI(MQTT_TAG, "sent subscribe successful, msg_id=%d\n\topic=%s",
				msg_id, topic)
		;
		//read offline data and send them if available
		if(medlo_sd_init()==ESP_OK){
			cJSON *offlineData = cJSON_CreateObject();
			cJSON *offlinePayloadArray = cJSON_CreateArray();
			cJSON *currentData = cJSON_CreateObject();
			FILE* f = fopen("/sdcard/PAYLOAD", "r");
			if (f != NULL) {
				char data[2048];
				fgets(data, sizeof(data), f);
				ESP_LOGW(STORAGE_TAG, "Read payload \n\n%s\n\n", data);
				fclose(f);
				//handle json
				offlineData = cJSON_Parse(data);
				offlinePayloadArray = cJSON_GetObjectItem(offlineData, "data");
				int i=0;
				int msg_id ;
				for(i=0;i<cJSON_GetArraySize(offlinePayloadArray);i++){
					currentData = cJSON_GetArrayItem(offlinePayloadArray,i);
					msg_id = esp_mqtt_client_publish(client, cJSON_GetStringValue(cJSON_GetObjectItem(currentData,"topic")), cJSON_GetStringValue(cJSON_GetObjectItem(currentData,"payload")), 0, 0, 0);
					ESP_LOGI(MQTT_TAG, "Offline saved data published with msg_id=%d to %s\n%s", msg_id,
							cJSON_GetStringValue(cJSON_GetObjectItem(currentData,"topic")), cJSON_GetStringValue(cJSON_GetObjectItem(currentData,"payload")));
				}
				medlo_sd_remove_payload();
			}
			medlo_sd_deinit();
		}
		break;
	case MQTT_EVENT_DISCONNECTED:
		ESP_LOGI(MQTT_TAG, "MQTT disconnected\nSwitching to offline if available")
		;
		led_green(1);
		led_blue(1);
		break;

	case MQTT_EVENT_SUBSCRIBED:
		ESP_LOGI(MQTT_TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id)
		;
		//sg_id = esp_mqtt_client_publish(client, "/topic/qos0", "data", 0, 0, 0);
		//ESP_LOGI(MQTT_TAG, "sent publish successful, msg_id=%d", msg_id);
		break;
	case MQTT_EVENT_UNSUBSCRIBED:
		ESP_LOGI(MQTT_TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id)
		;
		break;
	case MQTT_EVENT_PUBLISHED:
		ESP_LOGI(MQTT_TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id)
		;
		break;
	case MQTT_EVENT_DATA:
		ESP_LOGI(MQTT_TAG, "Receieved data on %s\n%s\n", event->topic,
				event->data)
		;
		cJSON *recievedObject = cJSON_Parse((char *) event->data);
		cJSON *maxTempObj = cJSON_GetObjectItem(recievedObject,
				"maxtemperature");
		cJSON *minTempObj = cJSON_GetObjectItem(recievedObject,
				"mintemperature");
		cJSON *maxHumidObj = cJSON_GetObjectItem(recievedObject, "maxhumidity");
		cJSON *minHumidObj = cJSON_GetObjectItem(recievedObject, "minhumidity");
		//render and save to NVS
		char * rendered = cJSON_Print(minTempObj);
		medlo_param_temp_min = atoi(rendered);
		nvs_set_i16(medlo_nvs_handle,ADDR_NVS_TEMP_MIN,medlo_param_temp_min);
		rendered = cJSON_Print(maxTempObj);
		medlo_param_temp_max = atoi(rendered);
		nvs_set_i16(medlo_nvs_handle,ADDR_NVS_TEMP_MAX,medlo_param_temp_max);
		rendered = cJSON_Print(minHumidObj);
		medlo_param_humid_min = atoi(rendered);
		nvs_set_i16(medlo_nvs_handle,ADDR_NVS_HUMID_MIN,medlo_param_humid_min);
		rendered = cJSON_Print(maxHumidObj);
		medlo_param_humid_max = atoi(rendered);
		nvs_set_i16(medlo_nvs_handle,ADDR_NVS_HUMID_MAX,medlo_param_humid_max);
		nvs_commit(medlo_nvs_handle);
		esp_restart();
		break;
	case MQTT_EVENT_ERROR:
		ESP_LOGI(MQTT_TAG, "MQTT_EVENT_ERROR")
		;
		break;
	}
	return ESP_OK;
}

esp_err_t mqtt_init() {
	const esp_mqtt_client_config_t mqtt_cfg = { .event_handle =
			mqtt_event_handler, .host = MEDLO_MQTT_SERVER, .port =
	MEDLO_MQTT_PORT, .client_id = MEDLO_DEVICE_ID, .username = authtoken,
			.password = "", .transport = MQTT_TRANSPORT_OVER_TCP,
			.disable_auto_reconnect = false, };

	client = esp_mqtt_client_init(&mqtt_cfg);
	esp_mqtt_client_start(client);

	return ESP_OK;
}

