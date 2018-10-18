#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <stdio.h>
#include <cJSON.h>

#include "sdkconfig.h"
#include "medlo_mqtt.h"
#include "medlo_time.h"
#include "medlo_payload.h"
#include "medlo_storage.h"


esp_err_t payload_send_sensor(char *sensor, double sensorvalue) {
	cJSON *postObject = cJSON_CreateObject();
	cJSON *eventObj = cJSON_CreateObject();
	cJSON *metaDataObj = cJSON_CreateObject();
	cJSON *payLoadObj = cJSON_CreateObject();

	ESP_LOGI(MQTT_TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
	//metadatabuild
	cJSON_AddItemToObject(metaDataObj, "owner", cJSON_CreateString("admin"));
	cJSON_AddItemToObject(metaDataObj, "deviceType",
			cJSON_CreateString("medlonode"));
	cJSON_AddItemToObject(metaDataObj, "deviceId",
			cJSON_CreateString(MEDLO_DEVICE_ID));
	cJSON_AddItemToObject(metaDataObj, "time",
			cJSON_CreateNumber(ntp_get_epoch()));
	//payLoad build
	cJSON_AddItemToObject(payLoadObj, sensor, cJSON_CreateNumber(sensorvalue));
	//create event
	cJSON_AddItemToObject(eventObj, "metaData", metaDataObj);
	cJSON_AddItemToObject(eventObj, "payloadData", payLoadObj);
	//post
	cJSON_AddItemToObject(postObject, "event", eventObj);
	char *jsonstring = cJSON_PrintUnformatted(postObject);
	//ESP_LOGI(MQTT_TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());
	cJSON_free(postObject);
	cJSON_free(eventObj);
	cJSON_free(metaDataObj);
	cJSON_free(payLoadObj);
	//ESP_LOGI(MQTT_TAG, "[APP] Free memory: %d bytes", esp_get_free_heap_size());

	//build topic and payload
	char payload[256];
	sprintf(payload, jsonstring);
	sprintf(topic, "carbon.super/medlonode/%s/%s", MEDLO_DEVICE_ID, sensor);
	int msg_id = esp_mqtt_client_publish(client, topic, payload, 0, 0, 0);
	if(msg_id==-1){//failed switch to offlinw if available
		if (medlo_sd_init() == ESP_OK) { //if microsd is available keep offline
			cJSON *offlineData = cJSON_CreateObject();
			cJSON *offlinePayloadArray = cJSON_CreateArray();
			cJSON *offlineDataNew = cJSON_CreateObject();
			ESP_LOGI(STORAGE_TAG, "Reading from SD card...");
			FILE* f = fopen("/sdcard/PAYLOAD", "r");
			if (f != NULL) {
				char data[2048];
				fgets(data, sizeof(data), f);
				ESP_LOGW(STORAGE_TAG, "Read payload \n\n%s\n\n", data);
				fclose(f);
				//handle json
				offlineData = cJSON_Parse(data);
				offlinePayloadArray = cJSON_GetObjectItem(offlineData,"data");
			}
			offlineData = cJSON_CreateObject();
			//create new item
			cJSON_AddItemToObject(offlineDataNew,"topic",cJSON_CreateString(topic));
			cJSON_AddItemToObject(offlineDataNew,"payload",cJSON_CreateString(payload));
			cJSON_AddItemToArray(offlinePayloadArray,offlineDataNew);
			//build all
			cJSON_AddItemToObject(offlineData,"data",offlinePayloadArray);
			char *jsonstring = cJSON_PrintUnformatted(offlineData);
			char offlineload[2048];
			sprintf(offlineload, "%s\n", jsonstring);
			//read existing
			medlo_sd_writeline(&offlineload);
			//finally deinit
			medlo_sd_deinit();
			ESP_LOGW(PAYLOAD_TAG, "Data saved offline %s\n%s",
								topic, payload);
		} else { //else restart
			esp_restart();
		}
	}else{
		ESP_LOGI(PAYLOAD_TAG, "Data published with msg_id=%d to %s\n%s", msg_id,
					topic, payload);
	}
	return ESP_OK;
}
