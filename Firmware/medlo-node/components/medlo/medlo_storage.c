#include <stdio.h>
#include <esp_err.h>
#include <freertos/FreeRTOS.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <esp_system.h>
#include "nvs_flash.h"
#include "nvs.h"
#include "esp_spiffs.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"
#include "medlo_mqtt.h"
#include "medlo_storage.h"

//NVS
esp_err_t medlo_nvs_init() {
	//NVS
	ESP_LOGI(STORAGE_TAG, "NVS opening....");
	esp_err_t err = nvs_flash_init();
	if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_READ_ONLY) {
		ESP_ERROR_CHECK(nvs_flash_erase()); // NVS partition was truncated and needs to be erased
		err = nvs_flash_init(); // Retry nvs_flash_init
	}
	err = nvs_open("storage", NVS_READWRITE, &medlo_nvs_handle);
	if (err != ESP_OK) {
		ESP_LOGE(STORAGE_TAG, "Error (%s) opening NVS handle!\n",
				esp_err_to_name(err));
	} else {
		ESP_LOGI(STORAGE_TAG, "NVS opened\n");
	}
	//return the result
	//err = nvs_get_i32(my_handle, "restart_counter", &restart_counter);
	//err = nvs_set_i32(my_handle, "restart_counter", restart_counter);
	return err;
}

//SPIFFS
esp_err_t medlo_spiffs_init() {
	ESP_LOGI(STORAGE_TAG, "Initializing SPIFFS");
	//mount
	esp_vfs_spiffs_conf_t conf = { .base_path = "/spiffs", .partition_label =
			"storage", .max_files = 5, .format_if_mount_failed = true };
	esp_err_t ret = esp_vfs_spiffs_register(&conf);
	if (ret != ESP_OK) {
		if (ret == ESP_FAIL) {
			ESP_LOGE(STORAGE_TAG, "Failed to mount or format filesystem");
		} else if (ret == ESP_ERR_NOT_FOUND) {
			ESP_LOGE(STORAGE_TAG, "Failed to find SPIFFS partition");
		} else {
			ESP_LOGE(STORAGE_TAG, "Failed to initialize SPIFFS (%s)",
					esp_err_to_name(ret));
		}
		return ret;
	}
	//partition info
	size_t total = 0, used = 0;
	ret = esp_spiffs_info(NULL, &total, &used);
	if (ret != ESP_OK) {
		ESP_LOGE(STORAGE_TAG, "Failed to get SPIFFS partition information (%s)",
				esp_err_to_name(ret));
	} else {
		ESP_LOGI(STORAGE_TAG, "Partition size: total: %d, used: %d", total,
				used);
	}
	return ret;
}

void medlo_spiffs_read_tokens() {
	//auth token
	FILE *f = fopen("/spiffs/authtoken", "r");
	if (f == NULL) {            //initialize with default
		sprintf(authtoken, MEDLO_AUTH_TOKEN);
		fclose(f);
		ESP_LOGW(STORAGE_TAG, "Using default auth token");
		f = fopen("/spiffs/authtoken", "w");
		fprintf(f, MEDLO_AUTH_TOKEN);
		fclose(f);
	} else {
		fgets(authtoken, sizeof(authtoken), f);
		fclose(f);
	}
	//refresh tokens
	f = fopen("/spiffs/refreshtoken", "r");
	if (f == NULL) {            //initialize with default
		sprintf(refreshtoken, MEDLO_REFRESH_TOKEN);
		fclose(f);
		ESP_LOGW(STORAGE_TAG, "Using default refresh token");
		f = fopen("/spiffs/refreshtoken", "w");
		fprintf(f, MEDLO_REFRESH_TOKEN);
		fclose(f);
	} else {
		fgets(refreshtoken, sizeof(refreshtoken), f);
		fclose(f);
	}
}

void medlo_spiffs_write_tokens() {
	//auth token
	FILE *f = fopen("/spiffs/authtoken", "w");
	fgets(authtoken, sizeof(authtoken), f);
	fclose(f);
	//refresh tokens
	f = fopen("/spiffs/refreshtoken", "w");
	fgets(refreshtoken, sizeof(refreshtoken), f);
	fclose(f);
}

//SD
esp_err_t medlo_sd_init() {
	ESP_LOGI(STORAGE_TAG, "Trying for offline SD storage (SPI)");
	sdmmc_host_t host = SDSPI_HOST_DEFAULT();
	sdspi_slot_config_t slot_config = SDSPI_SLOT_CONFIG_DEFAULT();
	slot_config.gpio_miso = PIN_NUM_MISO;
	slot_config.gpio_mosi = PIN_NUM_MOSI;
	slot_config.gpio_sck = PIN_NUM_CLK;
	slot_config.gpio_cs = PIN_NUM_CS;
	// formatted in case when mounting file system fails.
	esp_vfs_fat_sdmmc_mount_config_t mount_config = { .format_if_mount_failed =
	false, .max_files = 5, .allocation_unit_size = 16 * 1024 };
	esp_err_t ret = esp_vfs_fat_sdmmc_mount("/sdcard", &host, &slot_config,
			&mount_config, &sdcard);
	//if all good set status
	if (ret == ESP_OK) {
		sdmmc_card_print_info(stdout, sdcard);
	}
	return ret;
}

void medlo_sd_writeline(char *data) {
	ESP_LOGI(STORAGE_TAG, "Opening payload file.....");
	FILE* f = fopen("/sdcard/PAYLOAD", "w");
	if (f == NULL) {
		ESP_LOGE(STORAGE_TAG, "Failed to open file for writing");
	}
	fprintf(f, "%s\n", data);
	fclose(f);
	ESP_LOGI(STORAGE_TAG, "File written (size=%d)",sizeof(*data));
}

void medlo_sd_readline() {
	//auth token
	ESP_LOGI(STORAGE_TAG,"Reading from SD card...");
	FILE* f = fopen("/sdcard/PAYLOAD", "r");
	if (f != NULL) {
		char data[2048];
		fgets(data, sizeof(data), f);
		ESP_LOGW(STORAGE_TAG,"Read payload \n\n%s\n\n",data);
		fclose(f);
	}
}

void medlo_sd_remove_payload() {
	struct stat st;
	if (stat("/sdcard/payload.txt", &st) == 0) {
		unlink("/sdcard/payload.txt"); // Delete it if it exists
	}
}

esp_err_t medlo_sd_deinit() {
	return esp_vfs_fat_sdmmc_unmount();
}
