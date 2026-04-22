#ifndef HTTP_MANAGER_H
#define HTTP_MANAGER_H
#include <string.h>
#include "cJSON.h"

void http_get_request(const char *url, cJSON **response);
void http_post_request(const char *url, const cJSON *body, cJSON **response);
void http_keep_alive_request(const char *url, cJSON **response);

#endif
