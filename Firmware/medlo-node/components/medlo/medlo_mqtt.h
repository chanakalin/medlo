#include "mqtt_client.h"


//configurations
#define MEDLO_DEVICE_ID	CONFIG_MEDLO_DEVICE_ID
#define MEDLO_DEVICE_OWNER CONFIG_MEDLO_DEVICE_OWNER
#define MEDLO_AUTH_TOKEN CONFIG_MEDLO_AUTH_TOKEN
#define MEDLO_REFRESH_TOKEN CONFIG_MEDLO_REFRESH_TOKEN
#define MEDLO_APPLICATION_KEY CONFIG_MEDLO_APPLICATION_KEY
#define MEDLO_MQTT_SERVER CONFIG_MEDLO_MQTT_SERVER
#define MEDLO_MQTT_PORT CONFIG_MEDLO_MQTT_PORT
#define MEDLO_PUSH_INTERVAL CONFIG_MEDLO_PUSH_INTERVAL
//configurations end
#define MQTT_TAG "MQTT"


char authtoken[64];
char refreshtoken[64];
esp_mqtt_client_handle_t client;
char topic[256];

esp_err_t mqtt_init();




