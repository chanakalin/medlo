#include <string.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "esp_event_loop.h"
#include "esp_log.h"
#include "esp_system.h"
#include "nvs_flash.h"

#include "lwip/err.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/dns.h"

#include "esp_tls.h"

#include "token_refresh.h"

void https_refresh_tokens(void *pvParameters) {

	char buf[512];
	int ret, len;

	esp_tls_cfg_t cfg = { .cacert_pem_buf = server_root_cert_pem_start,
			.cacert_pem_bytes = server_root_cert_pem_end
					- server_root_cert_pem_start, };

	//struct esp_tls *tls = esp_tls_conn_http_new(WEB_URL, &cfg);
	struct esp_tls *tls = esp_tls_conn_new("medlo.trync.org",15,8243,&cfg);

	if (tls != NULL) {
		ESP_LOGI(HTTPS_TAG, "Connection established...");
	} else {
		ESP_LOGE(HTTPS_TAG, "Connection failed...");
	}

	ESP_LOGW(HTTPS_TAG,"Sending \n%s\n\n",REQUEST);
	size_t written_bytes = 0;
	do {
		ret = esp_tls_conn_write(tls, REQUEST + written_bytes,
				strlen(REQUEST) - written_bytes);
		if (ret >= 0) {
			ESP_LOGI(HTTPS_TAG, "%d bytes written", ret);
			written_bytes += ret;
		} else if (ret != MBEDTLS_ERR_SSL_WANT_READ
				&& ret != MBEDTLS_ERR_SSL_WANT_WRITE) {
			ESP_LOGE(HTTPS_TAG, "esp_tls_conn_write  returned 0x%x", ret);
		}
	} while (written_bytes < strlen(REQUEST));

	ESP_LOGI(HTTPS_TAG, "Reading HTTP response...");

	do {
		len = sizeof(buf) - 1;
		bzero(buf, sizeof(buf));
		ret = esp_tls_conn_read(tls, (char *) buf, len);

		if (ret == MBEDTLS_ERR_SSL_WANT_WRITE
				|| ret == MBEDTLS_ERR_SSL_WANT_READ)
			continue;

		if (ret < 0) {
			ESP_LOGE(HTTPS_TAG, "esp_tls_conn_read  returned -0x%x", -ret);
			break;
		}

		if (ret == 0) {
			ESP_LOGI(HTTPS_TAG, "connection closed");
			break;
		}

		len = ret;
		ESP_LOGD(HTTPS_TAG, "%d bytes read", len);
		/* Print response directly to stdout as it is read */
		for (int i = 0; i < len; i++) {
			putchar(buf[i]);
		}
	} while (1);

	esp_tls_conn_delete(tls);
}
