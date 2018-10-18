#include <esp_event.h>
#include <esp_event_loop.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include "esp_wps.h"
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <lwip/sockets.h>
#include <nvs_flash.h>
#include "driver/uart.h"
#include "driver/gpio.h"
#include <cJSON.h>
#include <sdkconfig.h>
#include <hwcrypto/sha.h>

#define MEDLO_TAG "MEDLO"

//configurations
#define WIFI_SSID CONFIG_MEDLO_WIFI_SSID
#define WIFI_PASSWORD CONFIG_MEDLO_WIFI_PASSWORD
#define PUSH_INTERVAL MEDLO_PUSH_INTERVAL*60000
//configurations end

//wifi
#ifndef PIN2STR
#define PIN2STR(a) (a)[0], (a)[1], (a)[2], (a)[3], (a)[4], (a)[5], (a)[6], (a)[7]
#define PINSTR "%c%c%c%c%c%c%c%c"
#endif

static esp_wps_config_t wps_config = WPS_CONFIG_INIT_DEFAULT(WPS_TYPE_PBC);

