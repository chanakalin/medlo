#define BME280_ADDRESS		0x76
#define BME280_TAG 			"BME280"

enum {
	BME280_REGISTER_DIG_T1 = 0x88,
	BME280_REGISTER_DIG_T2 = 0x8A,
	BME280_REGISTER_DIG_T3 = 0x8C,

	BME280_REGISTER_DIG_P1 = 0x8E,
	BME280_REGISTER_DIG_P2 = 0x90,
	BME280_REGISTER_DIG_P3 = 0x92,
	BME280_REGISTER_DIG_P4 = 0x94,
	BME280_REGISTER_DIG_P5 = 0x96,
	BME280_REGISTER_DIG_P6 = 0x98,
	BME280_REGISTER_DIG_P7 = 0x9A,
	BME280_REGISTER_DIG_P8 = 0x9C,
	BME280_REGISTER_DIG_P9 = 0x9E,

	BME280_REGISTER_DIG_H1 = 0xA1,
	BME280_REGISTER_DIG_H2 = 0xE1,
	BME280_REGISTER_DIG_H3 = 0xE3,
	BME280_REGISTER_DIG_H4 = 0xE4,
	BME280_REGISTER_DIG_H5 = 0xE5,
	BME280_REGISTER_DIG_H6 = 0xE7,

	BME280_REGISTER_CHIPID = 0xD0,
	BME280_REGISTER_VERSION = 0xD1,
	BME280_REGISTER_SOFTRESET = 0xE0,

	BME280_REGISTER_CONTROLHUMID = 0xF2,
	BME280_REGISTER_STATUS = 0xF3,
	BME280_REGISTER_CONTROL = 0xF4,
	BME280_REGISTER_CONFIG = 0xF5,
	BME280_REGISTER_PRESSUREDATA = 0xF7,
	BME280_REGISTER_TEMPDATA = 0xFA,
	BME280_REGISTER_HUMIDDATA = 0xFD,

	BME280_REGISTER_RESET = 0xE0,

	BME280_COMMAND_RESET = 0xB6
};

typedef struct {
	uint16_t dig_T1;
	int16_t dig_T2;
	int16_t dig_T3;

	uint16_t dig_P1;
	int16_t dig_P2;
	int16_t dig_P3;
	int16_t dig_P4;
	int16_t dig_P5;
	int16_t dig_P6;
	int16_t dig_P7;
	int16_t dig_P8;
	int16_t dig_P9;

	uint8_t dig_H1;
	int16_t dig_H2;
	uint8_t dig_H3;
	int16_t dig_H4;
	int16_t dig_H5;
	int8_t dig_H6;
} bme280_calib_data;

typedef uint32_t BME280_U32_t;
typedef int32_t BME280_S32_t;
typedef int64_t BME280_S64_t;

typedef struct {
	float temperature;
	float humidity;
	//float pressure;
} bme280_reading_data;

typedef struct {
	BME280_S32_t adc_P;
	BME280_S32_t adc_T;
	BME280_S32_t adc_H;
} bme280_adc;

BME280_S32_t t_fine;

bme280_calib_data bme280_calib;
esp_err_t bme280_init();
bme280_reading_data bme280_read_values();
float bme280_calculate_temperature(BME280_S32_t adc_T);
float bme280_calculate_humidity(BME280_S32_t adc_H);
