/**
 * @file
 *          http.cpp
 * @brief
 *          HttpCore class to connect to a Continental Cloud.
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
 *  This class send or get HTTP request to/from the Continental Cloud.
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

#include "connectivity/conti_http/http_core.h"
#include "connectivity/utils/curl/curlsync.h"

#include <json/json.h>

#include "core/log.h"


namespace vogon {
namespace connectivity {
namespace conti_http {

V_DECLARE_CLASS(HttpCore)

HttpCore::HttpCore(uv_loop_t* loop, const std::string& hostUrl,
                   bool verboseMode,
                   const std::string* noProxy, int timeout,
                   const std::vector<std::string>* customHeader,
                   bool chunkedFile):
    VObject(),
    m_loop(loop),
    m_hostUrl(hostUrl),
    m_lsc3Cred(nullptr),
    m_sslConf(nullptr),
    m_token(nullptr),
    m_customHeader(customHeader),
    m_chunkedFile(chunkedFile),
    m_verboseMode (verboseMode),
    m_noProxy(noProxy),
    m_timeout(timeout),
    m_mode(kCloudMode::NO_AUTH),
    m_curlResponseData(nullptr) {

    VLOG_CONSTRUCTOR();

    m_curl = new vcurl::CurlSync();

    m_curlResponsesCb = &HttpCore::curl_default_responses_write_callback;
}

HttpCore::HttpCore(uv_loop_t* loop, const std::string& hostUrl,
                   const vcurl::Lsc3Credentials& lsc3Cred,
                   bool verboseMode,
                   const std::string* noProxy, int timeout,
                   const std::vector<std::string>* customHeader,
                   bool chunkedFile):
    VObject(),
    m_loop(loop),
    m_hostUrl(hostUrl),
    m_lsc3Cred(&lsc3Cred),
    m_sslConf(nullptr),
    m_token(nullptr),
    m_customHeader(customHeader),
    m_chunkedFile(chunkedFile),
    m_verboseMode (verboseMode),
    m_noProxy(noProxy),
    m_timeout(timeout),
    m_mode(kCloudMode::LSC3),
    m_curlResponseData(nullptr) {

    VLOG_CONSTRUCTOR();

    m_curl = new vcurl::CurlSync();

    m_curlAuthCb = &HttpCore::curl_default_auth_write_callback;
    m_curlResponsesCb = &HttpCore::curl_default_responses_write_callback;
}

HttpCore::HttpCore(uv_loop_t* loop, const std::string& hostUrl,
                   const vcurl::SslConf& sslConf,
                   bool verboseMode,
                   const std::string* noProxy, int timeout,
                   const std::vector<std::string>* customHeader,
                   bool chunkedFile):
    VObject(),
    m_loop(loop),
    m_hostUrl(hostUrl),
    m_lsc3Cred(nullptr),
    m_sslConf(&sslConf),
    m_token(nullptr),
    m_customHeader(customHeader),
    m_chunkedFile(chunkedFile),
    m_verboseMode (verboseMode),
    m_noProxy(noProxy),
    m_timeout(timeout),
    m_mode(kCloudMode::VOEM),
    m_curlResponseData(nullptr) {

    VLOG_CONSTRUCTOR();

    m_curl = new vcurl::CurlSync();

    m_curlAuthCb = &HttpCore::curl_default_auth_write_callback;
    m_curlResponsesCb = &HttpCore::curl_default_responses_write_callback;
}

void HttpCore::setCurlResponseCb(size_t (*curlResponsesCb)(void* contents, size_t size, size_t nmemb, void* userdata),
                                 void* curlResponseData) {
    m_curlResponsesCb = curlResponsesCb;
    m_curlResponseData = curlResponseData;
}

void HttpCore::setCurlAuthCb(size_t (*curlAuthCb)(void* contents, size_t size, size_t nmemb, void* userdata)) {
    m_curlAuthCb = curlAuthCb;
}

HttpCore::~HttpCore() {
    VLOG_DESTRUCTOR();

    if (m_token != nullptr)
        delete m_token;
    m_token = nullptr;
}

size_t HttpCore::curl_default_auth_write_callback(void* contents, size_t size, size_t nmemb, void* data) {
    HttpCore* self = static_cast<HttpCore*>(data);

    std::string result ((char*)contents);
    VLOG_INFO_STATIC("HTTP Default Auth CallBack", "Response: {}", result);

    Json::Reader reader;
    Json::Value obj;
    reader.parse((char*)contents, obj);

    self->m_token = new vcurl::Token();
    self->m_token->access_token = obj["access_token"].asString();
    self->m_token->token_type = obj["token_type"].asString();
    self->m_token->expires_in = obj["expires_in"].asUInt();

    return size * nmemb;
}

size_t HttpCore::curl_default_responses_write_callback(void* contents, size_t size, size_t nmemb, void*) {
    std::string result ((char*)contents);
    VLOG_INFO_STATIC("HTTP Default Responses CallBack", "Response: {}", result);

    return size * nmemb;
}

bool HttpCore::connect() {
    return connect(m_verboseMode);
}

bool HttpCore::connect(bool verboseMode) {
    if (isAuthenticateNeeded() == false)
        return true;

    vcurl::RequestOut out;

    if (useToken()) {
        vcurl::RequestParams params;
        params.type = vcurl::kRequestType::POST_REQUEST;
        params.url = m_lsc3Cred->tokenUrl;
        params.noProxy = m_noProxy;
        params.timeout = m_timeout;

        params.verbose = verboseMode;

        params.authType = vcurl::CREDENTIALS;
        params.cred = m_lsc3Cred;

        std::string postData("grant_type=client_credentials");
        params.postData = &postData;

        out.curl_data = m_curlAuthCb;
        out.userdata = this;

        m_curl->handleRequest(params, out);
    }

    if (out.result == CURLE_OK)
        return true;
    else
        return false;
}

bool HttpCore::uploadFile(const std::string& url, const std::string& filename) {
    return uploadFile(url, filename, m_verboseMode);
}

bool HttpCore::uploadFile(const std::string& url, const std::string& filename,
                          bool verboseMode) {
    if (isAuthenticateNeeded()) {
        VLOG_WARN("You must get a token before sending request using the connect method");
        return false;
    }
    vcurl::RequestParams params;
    formatRequestParams(params, vcurl::kRequestType::POST_REQUEST, url, verboseMode);
    params.filePath = &filename;
    params.chunkedFile = m_chunkedFile;

    vcurl::RequestOut out;
    formatRequestOut(out);

    m_curl->handleRequest(params, out);

    if (out.result == CURLE_OK)
        return true;
    else
        return false;
}

bool HttpCore::sendSensoris(const std::string& url, const std::string& sensorisMsg) {
    return sendSensoris(url, sensorisMsg, m_verboseMode);
}

bool HttpCore::sendSensoris(const std::string& url, const std::string& sensorisMsg,
                            bool verboseMode) {
    return sendSensoris(url, "", sensorisMsg, verboseMode);
}

bool HttpCore::sendSensoris(const std::string& url, const std::string& header,
                            const std::string& sensorisMsg,
                            bool verboseMode) {

    if (isAuthenticateNeeded()) {
        VLOG_WARN("You must get a token before sending request using the connect method");
        return false;
    }
    vcurl::RequestParams params;
    params.header = header;
    formatRequestParams(params, vcurl::kRequestType::POST_REQUEST, url, verboseMode);
    params.postData = &sensorisMsg;

    vcurl::RequestOut out;
    formatRequestOut(out);

    m_curl->handleRequest(params, out);

    if (out.result == CURLE_OK)
        return true;
    else
        return false;
}

bool HttpCore::formatRequestParams(vcurl::RequestParams& params,
                                   vcurl::kRequestType requestType,
                                   const std::string& url,
                                   bool verbose) {
    std::string endPointUri;
    if (m_hostUrl.empty()) {
        endPointUri = url;
    } else {
        endPointUri = m_hostUrl + "/" + url;
    }

    params.type = requestType;
    params.url = endPointUri;
    params.noProxy = m_noProxy;
    params.timeout = m_timeout;

    params.verbose = verbose;

    if (useSsl() && m_sslConf != nullptr) {
        params.sslConf = m_sslConf;
    }

    if (useToken() && m_token != nullptr) {
        params.authType = vcurl::TOKEN;
        params.token = m_token;
    } else if (useCustomHeader()) {
        params.authType = vcurl::CUSTOM;
        params.customHeader = m_customHeader;
    } else {
        params.authType = vcurl::NO_AUTH;
    }
    return true;
}

bool HttpCore::formatRequestOut(vcurl::RequestOut& out) {
    out.curl_data = m_curlResponsesCb;
    out.userdata = m_curlResponseData;

    return true;
}

bool HttpCore::getData(const std::string& url) {
    return getData(url, m_verboseMode);
}

bool HttpCore::getData(const std::string& url,
                       bool verboseMode) {
    if (isAuthenticateNeeded()) {
        VLOG_WARN("You must get a token before sending request using the connect method");
        return false;
    }
    vcurl::RequestParams params;
    formatRequestParams(params, vcurl::kRequestType::GET_REQUEST, url, verboseMode);

    vcurl::RequestOut out;
    formatRequestOut(out);

    m_curl->handleRequest(params, out);

    if (out.result == CURLE_OK)
        return true;
    else
        return false;
}

} /* namespace conti_http */
} /* namespace connectivity */
} /* namespace vogon */
