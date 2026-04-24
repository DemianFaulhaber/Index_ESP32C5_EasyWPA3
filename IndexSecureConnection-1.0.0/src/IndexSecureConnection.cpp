#include "IndexSecureConnection.h"

#include <stdlib.h>
#include <string.h>

IndexSecureConnection::IndexSecureConnection() : creds_{} {
}

void IndexSecureConnection::copyField(char *dst, size_t dstSize, const char *src) {
    if (!dst || dstSize == 0) {
        return;
    }

    if (!src) {
        dst[0] = '\0';
        return;
    }

    strncpy(dst, src, dstSize - 1);
    dst[dstSize - 1] = '\0';
}

bool IndexSecureConnection::begin(const char *ssid,
                                  const char *identity,
                                  const char *password,
                                  const unsigned char *cl_cert,
                                  const unsigned char *cl_key,
                                  size_t cl_cert_len,
                                  size_t cl_key_len) {
    if (!ssid) {
        return false;
    }
    copyField(creds_.WIFI_SSID, sizeof(creds_.WIFI_SSID), ssid);  
    copyField(creds_.EAP_IDENTITY, sizeof(creds_.EAP_IDENTITY), identity);
    copyField(creds_.EAP_PASSWORD, sizeof(creds_.EAP_PASSWORD), password);
    
  
    // Note: DER binary data - do not use strlen; length must be provided by caller

    creds_.cl_cert = cl_cert;
    creds_.cl_key = cl_key;
    creds_.cl_cert_len = (int)cl_cert_len;
    creds_.cl_key_len = (int)cl_key_len;


    start_connection_process(&creds_);
    return get_status() == 'C';
}

IndexSecureConnection::Status IndexSecureConnection::status() const {
    STATUS_MACHINE raw = get_full_status();
    Status out{};

    out.code = raw.status;
    copyField(out.ip, sizeof(out.ip), raw.ip_address);
    copyField(out.error, sizeof(out.error), raw.error_message);

    return out;
}

bool IndexSecureConnection::ipAddress(char *outBuffer, size_t outLen) const {
    if (!outBuffer || outLen == 0) {
        return false;
    }

    outBuffer[0] = '\0';
    return get_ip_address(outBuffer, outLen) == 0;
}

bool IndexSecureConnection::jsonToBuffer(cJSON *json, char *outBuffer, size_t outLen) {
    if (!outBuffer || outLen == 0) {
        if (json) {
            cJSON_Delete(json);
        }
        return false;
    }

    outBuffer[0] = '\0';

    if (!json) {
        return false;
    }

    char *printed = cJSON_PrintUnformatted(json);
    cJSON_Delete(json);

    if (!printed) {
        return false;
    }

    copyField(outBuffer, outLen, printed);
    free(printed);
    return true;
}

bool IndexSecureConnection::get(const char *url, char *outBuffer, size_t outLen) const {
    cJSON *response = NULL;
    http_get_request(url, &response);
    return jsonToBuffer(response, outBuffer, outLen);
}

bool IndexSecureConnection::post(const char *url, const char *jsonBody, char *outBuffer, size_t outLen) const {
    cJSON *body = cJSON_Parse(jsonBody ? jsonBody : "{}");
    if (!body) {
        if (outBuffer && outLen > 0) {
            outBuffer[0] = '\0';
        }
        return false;
    }

    cJSON *response = NULL;
    http_post_request(url, body, &response);
    cJSON_Delete(body);

    return jsonToBuffer(response, outBuffer, outLen);
}

bool IndexSecureConnection::keepAlive(const char *url, char *outBuffer, size_t outLen) const {
    cJSON *response = NULL;
    http_keep_alive_request(url, &response);
    return jsonToBuffer(response, outBuffer, outLen);
}

#ifdef ARDUINO
String IndexSecureConnection::get(const String &url) const {
    char buffer[1024];
    if (!get(url.c_str(), buffer, sizeof(buffer))) {
        return String();
    }
    return String(buffer);
}

String IndexSecureConnection::post(const String &url, const String &jsonBody) const {
    char buffer[1024];
    if (!post(url.c_str(), jsonBody.c_str(), buffer, sizeof(buffer))) {
        return String();
    }
    return String(buffer);
}

String IndexSecureConnection::keepAlive(const String &url) const {
    char buffer[1024];
    if (!keepAlive(url.c_str(), buffer, sizeof(buffer))) {
        return String();
    }
    return String(buffer);
}
#endif
