#define SENSOR_SI1145 0
#define SENSOR_BME280 1

#define MEDLO_DATA_TAG "DATASEND"

static int16_t medlo_param_temp_max = 25;
static int16_t medlo_param_temp_min = 20;
static int16_t medlo_param_humid_max = 10;
static int16_t medlo_param_humid_min = 7;


void datasend_init();
void task_datasend();
void connect_mqtt();
