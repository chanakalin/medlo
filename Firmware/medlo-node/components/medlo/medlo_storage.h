#include "nvs_flash.h"
#include "nvs.h"
#include "esp_vfs_fat.h"
#include "driver/sdmmc_host.h"
#include "driver/sdspi_host.h"
#include "sdmmc_cmd.h"

#define ADDR_NVS_TEMP_MAX	"temp_max"
#define ADDR_NVS_TEMP_MIN	"temp_min"
#define ADDR_NVS_HUMID_MAX	"humid_max"
#define ADDR_NVS_HUMID_MIN	"humid_min"
#define ADDR_NVS_BOOT_COUNT	"boot_count"

#define STORAGE_TAG	"MEDLO STORAGE"
#define PIN_NUM_MISO 2
#define PIN_NUM_MOSI 15
#define PIN_NUM_CLK  14
#define PIN_NUM_CS   13

nvs_handle medlo_nvs_handle;
sdmmc_card_t* sdcard;

esp_err_t medlo_nvs_init();


esp_err_t medlo_spiffs_init();
void medlo_spiffs_read_tokens();
void medlo_spiffs_write_tokens();

esp_err_t medlo_sd_init();
void medlo_sd_writeline(char *data);
void medlo_sd_readline();
void medlo_sd_remove_payload();
esp_err_t medlo_sd_deinit();



