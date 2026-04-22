#ifndef EVENT_HANDLER_H
#define EVENT_HANDLER_H

#include <string.h>
#include "WIFI_SETUP.h"

typedef struct {
    char status;            // 'C' = connected, 'E' = error, 'D' = disconnected
    char ip_address[16];
    char error_message[128];
} STATUS_MACHINE;

void start_connection_process(credentials *creds);
STATUS_MACHINE get_full_status(void);
char get_status(void);
int get_ip_address(char *buffer, size_t len);

#endif


