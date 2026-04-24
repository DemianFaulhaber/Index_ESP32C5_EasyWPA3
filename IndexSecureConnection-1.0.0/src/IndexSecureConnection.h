#pragma once

#include <stdbool.h>
#include <stddef.h>

#ifdef ARDUINO
#include <Arduino.h>
#endif

extern "C" {
#include "WIFI_SETUP.h"
#include "EVENT_HANDLER.h"
#include "HTTP_MANAGER.h"
}

class IndexSecureConnection {
public:
    struct Status {
        char code;
        char ip[16];
        char error[128];

        bool connected() const {
            return code == 'C';
        }

        bool hasError() const {
            return code == 'E';
        }
    };

    IndexSecureConnection();

    bool begin(const char *ssid,
               const char *identity = nullptr,
               const char *password = nullptr,
               const unsigned char *cl_cert = nullptr,
               const unsigned char *cl_key = nullptr,
               size_t cl_cert_len = 0,
               size_t cl_key_len = 0);
    Status status() const;
    bool ipAddress(char *outBuffer, size_t outLen) const;

    bool get(const char *url, char *outBuffer, size_t outLen) const;
    bool post(const char *url, const char *jsonBody, char *outBuffer, size_t outLen) const;
    bool keepAlive(const char *url, char *outBuffer, size_t outLen) const;

#ifdef ARDUINO
    bool begin(const String &ssid, const String &identity, const String &password) {
        return begin(ssid.c_str(), identity.c_str(), password.c_str());
    }

    String get(const String &url) const;
    String post(const String &url, const String &jsonBody) const;
    String keepAlive(const String &url) const;
#endif

private:
    credentials creds_;

    static void copyField(char *dst, size_t dstSize, const char *src);
    static bool jsonToBuffer(cJSON *json, char *outBuffer, size_t outLen);
};
