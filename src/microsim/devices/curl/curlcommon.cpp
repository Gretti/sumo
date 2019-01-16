/**
 * @file
 *          curlcommon.cpp
 * @brief
 *          curlcommon util class
 *
 * @par Author (last changes):
 *          - Laurent Bousquet
 *          - Phone: +33 (0)6 37 85 83 21
 *          - Copyright (C) Continental Automotive GmbH 2018
 * @par Responsible Architect:
 *          - Laurent Bousquet
 *          - Copyright (C) Continental Automotive GmbH 2018
 * @par Platform:
 *      Flexbox/RPI/x86
 * @par OS:
 *      Linux
 *
 * @par SW-Component:
 *      Vogon
 * @par SW-Package:
 *      Connectivity / Curl
 * @par Bugs:
 *
 * @note
 *
 * @warning
 *
 * @par Description:
 *
 *  Common class to format curl requests
 *
 * @par Module-History:
 * @verbatim
 *  Date        Author             Reason
 *  06.13.2018   Laurent Bousquet  Initial Draft
 *
 *
 * @endverbatim
 * Copyright (C) Continental Automotive GmbH 2018
 * Alle Rechte vorbehalten. All Rights Reserved.
 * The reproduction, transmission or use of this document or its contents is
 * not permitted without express written authority.
 * Offenders will be liable for damages. All rights, including rights created
 * by patent grant or registration of a utility model or design, are reserved.
 */

#include "curlcommon.h"
#include "core/log.h"

#include "core/utils/base64.h"

#include <stdio.h>
#include <fcntl.h>

namespace vogon {
namespace connectivity {
namespace utils {
namespace curl {

static std::string kCurlCommonName("CurlCommon");

static
void dump(const char* text,
          FILE* stream, unsigned char* ptr, size_t size) {
    size_t i;
    size_t c;
    unsigned int width = 0x10;

    fprintf(stream, "%s, %10.10ld bytes (0x%8.8lx)\n",
            text, (long)size, (long)size);

    for (i = 0; i < size; i += width) {
        fprintf(stream, "%4.4lx: ", (long)i);

        /* show hex to the left */
        for (c = 0; c < width; c++) {
            if (i + c < size)
                fprintf(stream, "%02x ", ptr[i + c]);
            else
                fputs("   ", stream);
        }

        /* show data on the right */
        for (c = 0; (c < width) && (i + c < size); c++) {
            char x = (ptr[i + c] >= 0x20 && ptr[i + c] < 0x80) ? ptr[i + c] : '.';
            fputc(x, stream);
        }

        fputc('\n', stream); /* newline */
    }
}

static
int my_trace(CURL* handle, curl_infotype type,
             char* data, size_t size,
             void* userp) {
    const char* text;
    (void)handle; /* prevent compiler warning */
    (void)userp;

    switch (type) {
        case CURLINFO_TEXT:
            fprintf(stderr, "== Info: %s", data);
        default: /* in case a new one is introduced to shock us */
            return 0;

        case CURLINFO_HEADER_OUT:
            text = "=> Send header";
            break;
        case CURLINFO_DATA_OUT:
            text = "=> Send data";
            break;
        case CURLINFO_SSL_DATA_OUT:
            text = "=> Send SSL data";
            break;
        case CURLINFO_HEADER_IN:
            text = "<= Recv header";
            break;
        case CURLINFO_DATA_IN:
            text = "<= Recv data";
            break;
        case CURLINFO_SSL_DATA_IN:
            text = "<= Recv SSL data";
            break;
    }

    dump(text, stderr, (unsigned char*)data, size);
    return 0;
}

CURL* formatRequest(const RequestParams& in,
                    RequestOut& output) {
    CURL* curl = NULL;
    curl = curl_easy_init();

    curl_easy_setopt(curl, CURLOPT_URL, in.url.c_str());
    setCurlRequestType(curl, in.type);
    setCurlOptions(curl, in);

    setCurlHeaders(curl, in);

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, output.curl_data);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, output.userdata);

    if (in.filePath != nullptr) {
        FILE* fd;

        fd = fopen(in.filePath->c_str(), "rb");
        if (!fd) {
            VLOG_ERROR_STATIC(kCurlCommonName, "Can not open file: {}",
                              in.filePath->c_str());
            return curl; /* can't continue */
        }

        curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

        curl_easy_setopt(curl, CURLOPT_READDATA, fd);
        if (!in.chunkedFile) {
            struct stat file_info;
            if (fstat(fileno(fd), &file_info) != 0)
                return curl; /* can't continue */
            curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE, (curl_off_t)file_info.st_size);
        }

    } else if (in.postData != nullptr) {
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, in.postData->data());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, in.postData->size());
    }
    return curl;
}

void setCurlRequestType(CURL* curl, const kRequestType type) {
    std::string httpType = "GET";

    if (type == kRequestType::POST_REQUEST) {
        httpType = "POST";
    } else if (type == kRequestType::GET_REQUEST) {
        httpType = "GET";
    } else {
        VLOG_ERROR_STATIC(kCurlCommonName, "Invalid Request type requested");
        return;
    }

    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, httpType.c_str());
}

void setCurlHeaders(CURL* curl, const RequestParams& in) {
    struct curl_slist* headers = NULL;

    if (in.authType == CREDENTIALS) {
        headers = setCurlHeadersAuth(headers, *in.cred);
    } else if (in.authType == TOKEN) {
        headers = setCurlHeadersToken(headers, in.token);
    } else if (in.authType == CUSTOM) {
        headers = setCurlHeadersCustom(headers, *in.customHeader);
    }

    if (in.type == kRequestType::POST_REQUEST) {
        if(in.header != ""){
            headers = curl_slist_append(headers, in.header.c_str());
        } else{
            headers = curl_slist_append(headers, "content-type: application/octet-stream");
        }
    }

    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
}

struct curl_slist* setCurlHeadersAuth(curl_slist* headers, const Lsc3Credentials& in) {
    std::string basicauth64 = vutils::Base64::encode(in.username + ":" + in.password);
    std::string basicauthstring = "authorization: Basic " + basicauth64;

    headers = curl_slist_append(headers, basicauthstring.c_str());
    headers = curl_slist_append(headers, "content-type: application/x-www-form-urlencoded");
    return headers;
}

struct curl_slist* setCurlHeadersToken(curl_slist* headers, const Token* token) {
    std::string tokenauthstring = "authorization: " + token->token_type + " " + token->access_token;

    headers = curl_slist_append(headers, tokenauthstring.c_str());
    return headers;
}

struct curl_slist* setCurlHeadersCustom(curl_slist* headers, const std::vector<std::string>& customHeader) {
    for (std::string h : customHeader) {
        headers = curl_slist_append(headers, h.c_str());
    }
    return headers;
}

void setCurlOptions(CURL* curl, const RequestParams& in) {
    VLOG_INFO_STATIC(kCurlCommonName, "Set Curl Options");

    setCurlOptionsVerbose(curl, in);
    setCurlOptionsTimeout(curl, in);
    setCurlOptionsProxy(curl, in);
    setCurlOptionsSsl(curl, in);
}

void setCurlOptionsVerbose(CURL* curl, const RequestParams& in) {
    VLOG_INFO_STATIC(kCurlCommonName, "Set Curl Verbose Options:");

    if (in.verbose) {
        VLOG_INFO_STATIC(kCurlCommonName, "Verbose mode: Enabled");
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    } else {
        VLOG_DEBUG_STATIC(kCurlCommonName, "Verbose mode: Disabled");
    }

    if (in.debug) {
        /* Activate the header response - warning this is displayed using the writecallback */
        curl_easy_setopt(curl, CURLOPT_HEADER, 1L);

        /* example.com is redirected, so we tell libcurl to follow redirection */
        curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

        VLOG_INFO_STATIC(kCurlCommonName, "Debug and Verbose modes: Enabled");
        curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, my_trace);

        /* the DEBUGFUNCTION has no effect until we enable VERBOSE */
        curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    } else {
        VLOG_DEBUG_STATIC(kCurlCommonName, "Debug mode: Disabled");
    }
}

void setCurlOptionsTimeout(CURL* curl, const RequestParams& in) {
    VLOG_INFO_STATIC(kCurlCommonName, "Request TimeOut: {}", in.timeout);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, in.timeout);
}

void setCurlOptionsProxy(CURL* curl, const RequestParams& in) {
    if (in.noProxy) {
        VLOG_INFO_STATIC(kCurlCommonName, "Set Curl Proxy Options:");

        VLOG_INFO_STATIC(kCurlCommonName, "Set noProxy to: {}", in.noProxy->c_str());
        curl_easy_setopt(curl, CURLOPT_NOPROXY, in.noProxy->c_str());
    } else {
        VLOG_INFO_STATIC(kCurlCommonName, "No Curl Proxy Options");
    }
}

void setCurlOptionsSsl(CURL* curl, const RequestParams& in) {
    VLOG_DEBUG_STATIC(kCurlCommonName, "Disable certificate's name against host check");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    if (in.sslConf) {
        VLOG_INFO_STATIC(kCurlCommonName, "Set Curl SSL Options:");

        VLOG_INFO_STATIC(kCurlCommonName, "Set cert file to: {}", in.sslConf->certFilePath.c_str());
        curl_easy_setopt(curl, CURLOPT_SSLCERT, in.sslConf->certFilePath.c_str());
        VLOG_INFO_STATIC(kCurlCommonName, "Set key file to: {}", in.sslConf->keyFilePath.c_str());
        curl_easy_setopt(curl, CURLOPT_SSLKEY, in.sslConf->keyFilePath.c_str());
        VLOG_INFO_STATIC(kCurlCommonName, "Set CA file to: {}", in.sslConf->caCertFilePath.c_str());
        curl_easy_setopt(curl, CURLOPT_CAINFO, in.sslConf->caCertFilePath.c_str());

        VLOG_DEBUG_STATIC(kCurlCommonName, "Enable certificate's name against peer check");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L);
    } else {
        VLOG_INFO_STATIC(kCurlCommonName, "No Curl SSL Options");
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    }
}

} /* namespace curl */
} /* namespace utils */
} /* namespace connectivity */
} /* namespace vogon */
