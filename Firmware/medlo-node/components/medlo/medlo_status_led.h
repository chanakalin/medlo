
#define LED_BLUE	23
#define LED_GREEN	22
#define WPS_SWITCH	32

void medlo_gpio_init();
void led_green(uint8_t status);
void led_blue(uint8_t status);
uint8_t wps_switch_status();
