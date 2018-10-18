#include <string.h>
#include <stdlib.h>

#define HTTPS_TAG "HTTPS_TAG"

extern const uint8_t server_root_cert_pem_start[] asm("_binary_server_root_cert_pem_start");
extern const uint8_t server_root_cert_pem_end[]   asm("_binary_server_root_cert_pem_end");

#define WEB_SERVER "medlo.trync.org:8243"
#define WEB_PORT "8243"
#define WEB_URL "https://medlo.trync.org:8243/token"

static const char *REQUEST = "POST /token HTTP/1.0\r\n"
    "Host: medlo.trync.org:8243\r\n"
    "User-Agent: esp-idf/1.0 esp32\r\n"
	"Authorization: Basic RWZYWTQyc1pNUlBlODllcWgzSUhRdkhscXJFYTpxZnVvc0dTZVpINDU4UmZlaldxMGpMTTVXS1Vh\r\n"
	"Content-Type: application/x-www-form-urlencoded\r\n"
	"Content-Length: 78\r\n"
	"\r\n"
	"grant_type=password&username=admin&password=admin&scope=perm:medlonode:enroll\r\n";

void https_refresh_tokens();
