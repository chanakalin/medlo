#include <esp_event.h>
#include <esp_event_loop.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <esp_err.h>
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <lwip/sockets.h>
//time
#include "lwip/err.h"
#include "lwip/apps/sntp.h"
#include <time.h>
#include <sys/time.h>

#include "medlo_time.h"

void ntp_init_time() {
	sntp_setoperatingmode(SNTP_OPMODE_POLL);
	sntp_setservername(0, "pool.ntp.org");
	sntp_setservername(1, "0.asia.pool.ntp.org");
	sntp_setservername(2, "1.asia.pool.ntp.org");
	sntp_setservername(3, "3.asia.pool.ntp.org");
	sntp_init();

	char strftime_buf[64];

	time_t now;
	struct tm timeinfo;
	time(&now);
	localtime_r(&now, &timeinfo);
	// Is time set? If not, tm_year will be (1970 - 1900).
	if (timeinfo.tm_year < (2016 - 1900)) {
		ESP_LOGI(MEDLO_TIME_TAG,
				"Time is not set yet. Connecting to WiFi and getting time over NTP.");
		time_t now = 0;
		struct tm timeinfo = { 0 };
		int retry = 0;
		const int retry_count = 10;
		while (timeinfo.tm_year < (2016 - 1900) && ++retry < retry_count) {
			ESP_LOGI(MEDLO_TIME_TAG,
					"Waiting for system time to be set... (%d/%d)", retry,
					retry_count);
			time(&now);
			localtime_r(&now, &timeinfo);
			vTaskDelay(2000 / portTICK_PERIOD_MS);
		}
		time(&now);
		localtime_r(&now, &timeinfo);
		strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
		ESP_LOGI(MEDLO_TIME_TAG, "The current date/time (GMT) is: %s",
				strftime_buf);
		time(&now);
	}
}

double ntp_get_epoch() {
	time_t now;
	struct tm timeinfo;
	time(&now);
	struct tm epoch = { 0 };
	epoch.tm_hour = 0;
	epoch.tm_min = 0;
	epoch.tm_sec = 0;
	epoch.tm_year = 70;
	epoch.tm_mon = 0;
	epoch.tm_mday = 1;
	//LK timezone
	setenv("TZ", "GMT-5:30", 1);
	tzset();
	localtime_r(&now, &timeinfo);
	return difftime(mktime(&timeinfo), mktime(&epoch));
}

