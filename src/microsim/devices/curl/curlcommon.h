/*******************************************************************************
 * Copyright (C) Continental Automotive GmbH 2018
 * Alle Rechte vorbehalten. All Rights Reserved.
 * The reproduction, transmission or use of this document or its contents is not
 * permitted without express written authority. Offenders will be liable for
 * damages. All rights, including rights created by patent grant or registration
 * of a utility model or design, are reserved.
 ******************************************************************************/
#ifndef VOGON_CONNECTIVITY_UTILS_CURL_CURLCOMMON_H_
#define VOGON_CONNECTIVITY_UTILS_CURL_CURLCOMMON_H_

/**
 * @file
 *          curlcommon.h
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

#include <curl/curl.h>

#include <vector>

#include <uv.h>

#include "core/vobject.h"

#include <assert.h>

namespace vogon {
namespace connectivity {
namespace utils {
namespace curl {

/*!
 * \brief The kRequestAuthType enum indicate which authentification is used for the request.
 */
enum kRequestAuthType {
    CREDENTIALS = 0,            /*!< Credentials requires a tuple (username, password) */
    TOKEN,                      /*!< Token requires a token to identify a HTTP session */
    SSL,                        /*!< SSL specifies a tuple (ca certificate, certificate and private key) */
    NO_AUTH,                    /*!< No authentication */
    CUSTOM,                     /*!< Custom Headers authentication */
    REQUEST_AUTH_TYPES_COUNT
};

/*!
 * \brief The kRequestType enum indicates the HTTP request type.
 */
enum class kRequestType {
    POST_REQUEST = 0,           /*!< POST request to push a data. The response can contain data readable with request_out write callback.*/
    GET_REQUEST,                /*!< GET request to get data from the HTTP server. The response contains data readable with request_out write callback. */
    REQUEST_TYPES_COUNT
};

/*!
 * \brief The Token struct represents a token.
 */
struct Token {
    std::string access_token;   /*!< access token content string to pass to the cloud. */
    std::string token_type;     /*!< token type. */
    uint32_t expires_in;        /*!< token expiration time. */
};
/*!
 * \brief The Lsc3Credentials struct stores LSC3 credentials.
 */
struct Lsc3Credentials {
    /*!
     * \brief Lsc3Credentials Empty constructor.
     */
    Lsc3Credentials() :
        username(),
        password(),
        tokenUrl()
    {}
    /*!
     * \brief Lsc3Credentials Constructor.
     * \param user  Credential username.
     * \param pass  Credential password.
     * \param url   Url to get the token.
     */
    Lsc3Credentials(const std::string& user,
                    const std::string& pass,
                    const std::string& url) :
        username(user),
        password(pass),
        tokenUrl(url)
    {}

    /*!
     * \brief LSC3 Credential username
     */
    std::string username;
    /*!
     * \brief LSC3 Credential password
     */
    std::string password;
    /*!
     * \brief LSC3 Credential tokenUrl.
     *
     * This url is joined to get the token matching with the username and password Credential.
     */
    std::string tokenUrl;
};
/*!
 * \brief The SslConf struct stores the ssl Certificates file pathes.
 *
 * The SslConf contains:
 * - Authority Certificate file who validate the Client Keys Certificate.
 * - Client Certificate who validate the Client keys.
 * - Client private key.
 *
 * All thoses files are representated by there pathes.
 */
struct SslConf {
    /*!
     * \brief SslConf Empty Constructor.
     */
    SslConf() {}
    /*!
     * \brief SslConf Constructor
     * \param cert Client keys Certificate.
     * \param key Client private key
     * \param caCert Certificate Authority
     */
    SslConf(const std::string& cert, const std::string& key, const std::string& caCert) :
        certFilePath(cert),
        keyFilePath(key),
        caCertFilePath(caCert)
    {}

    /*!
     * \brief certFilePath Client keys Certificate file path.
     *
     * Generally it is a pem file.
     */
    std::string certFilePath;
    /*!
     * \brief keyFilePath Client private key file path.
     */
    std::string keyFilePath;
    /*!
     * \brief caCertFilePath Authority Certificate file path
     *
     * Generally it is a pem file.
     */
    std::string caCertFilePath;
};

struct RequestParams {
    RequestParams() :
        header(""),
        type(kRequestType::POST_REQUEST),
        noProxy(nullptr),
        timeout(0),
        verbose(false),
        debug(false),
        authType(NO_AUTH),
        cred(nullptr),
        sslConf(nullptr),
        token (nullptr),
        filePath(nullptr),
        postData(nullptr),
        customHeader(nullptr),
        chunkedFile(true)
    {}

    std::string header;
    //Request endpoint informations
    /*!
     * \brief Request type.
     */
    kRequestType type;
    /*!
     * \brief Request Endpoint url
     */
    std::string url;

    /*!
     * \brief noProxy variable which indicates when to ignore the proxy configured.
     *
     * Please set a host list separate by a ";".
     *
     * - If it is null the value is not passed to curl options.
     * - If empty the all urls will use the proxy.
     * - If "http://hostUrl;http://hostUrl2" hostUrl and hostUrl2 won't use the proxy.
     * - If "*" all site won't use the proxy
     */
    const std::string* noProxy;
    /*!
     * \brief timeout in second. If no reply occured before the timeout an error will be thrown.
     */
    int timeout;
    /*!
     * \brief verbose Mode. The verbose mode is readable in the write callback.
     *
     * Debug mode print http request and response.
     */
    bool verbose;
    /*!
     * \brief Debug Mode. The verbose mode is readable in the write callback.
     */
    bool debug;

    //Request authentication informations
    /*!
     * \brief Authentication request type.
     */
    kRequestAuthType authType;
    /*!
     * \brief Credentials to use to perform the request.
     */
    const Lsc3Credentials* cred;
    /*!
     * \brief sslConf to use to perform the request.
     */
    const SslConf* sslConf;
    /*!
     * \brief token to use to perform the request.
     */
    Token* token;

    /*!
     * \brief filePath If not nullptr this path will be uploaded
     */
    const std::string* filePath;
    /*!
     * \brief postData If not nullptr this data will be upload in the data request field
     */
    const std::string* postData;
    /*!
     * \brief customHeader If not empty this vector will contain the custom headers required for the request
     */
    const std::vector<std::string>* customHeader;
    /*!
     * \brief chunkedFile If false the upload will be based on the filesize parameter
     */
    bool chunkedFile;
};

/*!
 * \brief The RequestOut struct to use to get the result
 */
struct RequestOut {
    /*!
     * \brief Callback to pass to be able to read the result.
     */
    size_t (*curl_data)(void *contents, size_t size, size_t nmemb, void* userdata);
    /*!
     * \brief userdata to pass to the callback.
     */
    void* userdata;
    /*!
     * \brief Curl code result. (CURLE_OK if succeed)
     */
    CURLcode result;
};

/*!
 * \brief formatRequest Return the curl request to perform.
 * \param in Curl Request in parameters.
 * \param output Curl Request out parameters.
 * \return The formatted Curl request.
 */
CURL* formatRequest(const RequestParams& in,
                    RequestOut& output);

/*!
 * \brief setCurlRequestType Set the curl request type
 * \param curl Curl request to update.
 * \param type Curl request type.
 */
void setCurlRequestType(CURL* curl, const kRequestType type);
/*!
 * \brief setCurlOptions Set the curl request options
 * \param curl Curl request to update
 * \param in Curl request In parameters
 */
void setCurlOptions(CURL* curl, const RequestParams &in);
/*!
 * \brief setCurlOptionsVerbose Set the curl verbose options
 * \param curl Curl request to update
 * \param in Curl request In parameters
 */
void setCurlOptionsVerbose(CURL* curl, const RequestParams &in);
/*!
 * \brief setCurlOptionsTimeout Set the curl timeout option
 * \param curl Curl request to update
 * \param in Curl request In parameters
 */
void setCurlOptionsTimeout(CURL* curl, const RequestParams &in);
/*!
 * \brief setCurlOptionsSsl Set the curl ssl options
 * \param curl Curl request to update
 * \param in Curl request In parameters
 */
void setCurlOptionsSsl(CURL* curl, const RequestParams &in);
/*!
 * \brief setCurlOptionsProxy Set the curl proxy option
 * \param curl Curl request to update
 * \param in Curl request In parameters
 */
void setCurlOptionsProxy(CURL* curl, const RequestParams &in);

/*!
 * \brief setCurlHeaders Set the curl request headers
 * \param curl Curl request to update
 * \param in Curl request In parameters
 */
void setCurlHeaders(CURL* curl, const RequestParams& in);
/*!
 * \brief setCurlHeadersAuth Set the curl request header authentification options.
 * \param headers Header to update.
 * \param cred Credentials to use.
 * \return
 */
struct curl_slist *setCurlHeadersAuth(curl_slist *headers, const Lsc3Credentials& cred);
/*!
 * \brief setCurlHeadersToken Set the curl request header token options.
 * \param headers Header to update.
 * \param token Token to use.
 * \return
 */
struct curl_slist *setCurlHeadersToken(curl_slist *headers, const Token* token);

/*!
 * \brief setCurlHeadersCustom Set the curl request header custom options.
 * \param headers Header to update.
 * \param customHeader The vector of headers to use.
 * \return
 */
struct curl_slist *setCurlHeadersCustom(curl_slist* headers, const std::vector<std::string>& customHeader);
} /* namespace curl */
} /* namespace utils */
} /* namespace connectivity */
} /* namespace vogon */

#endif // VOGON_CONNECTIVITY_UTILS_CURL_CURLCOMMON_H_
