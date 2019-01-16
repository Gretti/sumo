/**
 * @file
 *          http.h
 * @brief
 *          Http class to connect to a Continental Cloud.
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

#ifndef VOGON_CONNECTIVITY_CONTI_HTTP_HTTP_CORE_H_
#define VOGON_CONNECTIVITY_CONTI_HTTP_HTTP_CORE_H_

#include <string>
#include <vector>

#include "core/vobject.h"
#include "connectivity/conti_http/conti_http.h"
#include "connectivity/utils/utils.h"

#include <uv.h>

namespace vogon {
namespace connectivity {
namespace utils {
namespace curl {

class CurlSync;
class Lsc3Credentials;
class SslConf;
class Token;
class RequestParams;
class RequestOut;
enum class kRequestType;

} /* namespace curl */
} /* namespace utils */
} /* namespace connectivity */
} /* namespace vogon */

namespace vcurl = vogon::connectivity::utils::curl;

namespace vogon {
namespace connectivity {
namespace conti_http {

/*!
 * \brief The HttpCore class manages an Http connection and its session.
 *
 * The HTTP Connection is associated to the an host URL. <STRONG>If the host URL is empty
 * it must be specified in the send methods URL.</STRONG>
 *
 * The HttpConnection can be LSC3 or VOEM or NO_AUTH depending the session.
 *
 * \section HttpCore_Lsc3 LSC3 method
 *
 * The connect method will get the connection token at the defined Lsc3Credentials settings.
 *
 * You can sent any requests as long as a token was not obtained. This avoid to send rejected requests.
 *
 * \section HttpCore_Voem VOEM method
 *
 * The connect method won't do anything
 *
 * For the VOEM connection, the ssl confirguration will be sent at each request.
 *
 * \section HttpCore_Curl Curl Settings
 *
 * The Connection manage 3 curl settings:
 *
 *   - verbosity: set to false by default
 *   - noProxy: set to use the system proxy by default
 *   - timeout: set the requests timeout (0 - infinite by default)
 *
 * \section HttpCore_Usage Http connection usage
 *
 *   - The connect() method should be sent at init time.
 *   - You can use sendMessage, UploadFiles, getData to send requests. Their
 * returns will indicate if the requests have succeed. To parse the content you can
 * set a write callback thanks to setCurlResponseCb() method. The writecallback method can be called
 * many time for a request depending the post data size sent by the server.
 *
 */
class HttpCore : public vcore::VObject {

  public:
    /*!
     * \brief The kCloudMode enum Supported Connections types to cloud
     */
    enum kCloudMode {
        NO_AUTH = 0,
        LSC3,
        VOEM,
        CLOUD_HTTP_MODES_COUNT
    };

    /*!
     * \brief Managed Http Cloud Connection Backend
     * \param loop Vogon uv loop to use.
     * \param hostUrl Host URL <STRONG>Without trailing "/" character</STRONG>
     * \param verboseMode Activate or not curl verbose mode
     * \param noProxy No proxy curl parameter.
     * \param timeout Curl request timeout option
     * \param customHeader Vector of custom HTTP headers
     * \param chunkedFile Upload a chunked or complete file
     */
    HttpCore(uv_loop_t* loop, const std::string& hostUrl,
             bool verboseMode = false,
             const std::string* noProxy = nullptr, int timeout = 0,
             const std::vector<std::string>* customHeader = nullptr,
             bool chunkedFile = true);

    /*!
     * \brief Managed Http Cloud Connection Backend
     * \param loop Vogon uv loop to use.
     * \param hostUrl Host URL <STRONG>Without trailing "/" character</STRONG>
     * \param lsc3Cred credentials for LSC3 connection
     * \param verboseMode Activate or not curl verbose mode
     * \param noProxy No proxy curl parameter.
     * \param timeout Curl request timeout option
     * \param customHeader Vector of custom HTTP headers
     * \param chunkedFile Upload a chunked or complete file
     */
    HttpCore(uv_loop_t* loop, const std::string& hostUrl,
             const vcurl::Lsc3Credentials& lsc3Cred,
             bool verboseMode = false,
             const std::string* noProxy = nullptr, int timeout = 0,
             const std::vector<std::string>* customHeader = nullptr,
             bool chunkedFile = true);
    /*!
     * \brief Managed Http Cloud Connection Backend
     * \param loop Vogon uv loop to use.
     * \param hostUrl Host URL <STRONG>Without trailing "/" character</STRONG>
     * \param sslConf SSL Configuration for VOEM Connection
     * \param verboseMode Activate or not curl verbose mode
     * \param noProxy No proxy curl parameter.
     * \param timeout Curl request timeout option
     * \param customHeader Vector of custom HTTP headers
     * \param chunkedFile Upload a chunked or complete file
     */
    HttpCore(uv_loop_t* loop, const std::string& hostUrl,
             const vcurl::SslConf& sslConf,
             bool verboseMode = false,
             const std::string* noProxy = nullptr, int timeout = 0,
             const std::vector<std::string>* customHeader = nullptr,
             bool chunkedFile = true);
    virtual ~HttpCore();

    /*!
     * \brief connect Perform the connection session authentication.
     * \return true if the session request has succeed otherwise return false.
     *
     * The request verbosity is set as the connection configuration.
     */
    bool connect();

    /*!
     * \brief connect Perform the connection session authentication.
     * \param verboseMode Specific Request verbose configuration.
     * \return true if the session request has succeed otherwise return false.
     *
     * It does nothing if the hostUrl is empty.
     *
     * verboseMode parameter can override the connection configuration.
     */
    bool connect(bool verboseMode);

    /*!
     * \brief uploadFile Upload a file to the configured cloud.
     * \param url endPoint URL to concatenate with the host URL.
     * \param filename Filename to upload.
     * \return true if CURLE_OK has been returned otherwise return false.
     *
     * <STRONG>It must not start with a trailing "/" character.</STRONG>. It is concatenated
     * with the m_host parameters.
     *
     * The request verbosity is set as the connection configuration.
     */
    bool uploadFile(const std::string& url, const std::string& filename);
    /*!
     * \brief uploadFile Upload a file to the configured cloud.
     * \param url endPoint URL to concatenate with the host URL.
     * \param filename Filename to upload.
     * \param verboseMode Specific Request verbose configuration.
     * \return true if CURLE_OK has been returned otherwise return false.
     *
     * <STRONG>It must not start with a trailing "/" character.</STRONG>. It is concatenated
     * with the m_host parameters.
     *
     * Verbose Mode parameter can override the connection configuration.
     */
    bool uploadFile(const std::string& url,
                    const std::string& filename,
                    bool verboseMode);

    /*!
     * \brief sendSensoris Send a sensoris message to the configured cloud.
     * \param url endPoint URL to concatenate with the host URL or it uses it if host URL is empty.
     * \param sensorisMsg Sensoris message to upload.
     * \return true if CURLE_OK has been returned otherwise return false.
     *
     * <STRONG>It must not start with a trailing "/" character.</STRONG>. It is concatenated
     * with the m_host parameters.
     *
     * The request verbosity is set as the connection configuration.
     */
    bool sendSensoris(const std::string& url, const std::string& sensorisMsg);
    /*!
     * \brief sendSensoris Send a sensoris message to the configured cloud.
     * \param url endPoint URL to concatenate with the host URL or it uses it if host URL is empty.
     * \param sensorisMsg Sensoris message to upload.
     * \param verboseMode Specific Request verbose configuration.
     * \return true if CURLE_OK has been returned otherwise return false.
     *
     * <STRONG>It must not start with a trailing "/" character.</STRONG>. It is concatenated
     * with the m_host parameters.
     *
     * verbose parameter can override the connection configuration.
     */
    bool sendSensoris(const std::string& url,
                      const std::string& sensorisMsg,
                      bool verboseMode);

    bool sendSensoris(const std::string& url, const std::string& header,
                                const std::string& sensorisMsg,
                                bool verboseMode);

    /*!
     * \brief getData Get data from the configured cloud.
     * \param url endPoint URL to concatenate with the host URL or it uses it if host URL is empty.
     * \return true if CURLE_OK has been returned otherwise return false.
     *
     * <STRONG>It must not start with a trailing "/" character.</STRONG>. It is concatenated
     * with the m_host parameters.
     *
     * The request verbosity is set as the connection configuration.
     */
    bool getData(const std::string& url);
    /*!
     * \brief getData Get data from the configured cloud.
     * \param url endPoint URL to concatenate with the host URL or it uses it if host URL is empty.
     * \param verboseMode Specific Request verbose configuration.
     * \return true if CURLE_OK has been returned otherwise return false.
     *
     * <STRONG>It must not start with a trailing "/" character.</STRONG>. It is concatenated
     * with the m_host parameters.
     *
     * verbose parameter can override the connection configuration.
     */
    bool getData(const std::string& url,
                 bool verboseMode);

    const std::string& className()const;

    /*!
     * \brief isAuthenticateNeeded Return if an authentication is needed or not.
     * \return true if the connection requires a Session authentication and if it is already
     * authenticated otherwise return false.
     */
    bool isAuthenticateNeeded() const {
        if (!useToken())
            return false;
        else
            return !hasToken();
    }

    /*!
     * \brief useToken Indicates if the session use a token or not.
     * \return true if a token is requested otherwise return false.
     */
    bool useToken() const {
        return (m_mode == LSC3);
    }
    /*!
     * \brief hasToken Indicates if the connection already get the session token.
     * \return true if a token is configured otherwise return false.
     */
    bool hasToken() const {
        return m_token != nullptr;
    }

    /*!
     * \brief useSsl Indicates if the connection use Ssl or not.
     * \return true if the connection use ssl otherwise return false.
     */
    bool useSsl() const {
        return (m_mode == VOEM);
    }

    /*!
     * \brief useCustomHeader Indicates if the Http need custom header for the request or not.
     * \return true if a vector of custom headers is initialized and not empty otherwise return false.
     */
    bool useCustomHeader() const {
        return m_customHeader != nullptr;
    }

    /*!
     * \brief hostUrl Return the Connection configured host url.
     * \return return the Connection host URL.
     *
     * <STRONG>It must not finish with a trailing "/" character.</STRONG>.
     */
    const std::string& hostUrl()const {
        return m_hostUrl;
    }

    /*!
     * \brief lsc3Credentials Lsc3Credentials configured.
     * \return nullptr if not configured otherwise return Lsc3Credentials.
     *
     * Lsc3 Credentials are used for LSC3 connection.
     */
    const vcurl::Lsc3Credentials* lsc3Credentials()const {
        return m_lsc3Cred;
    }
    /*!
     * \brief sslConf Ssl configuration.
     * \return nullptr if not configured otherwise return SslConfiguration.
     *
     * Ssl Configuration are used for VOEM connection.
     */
    const vcurl::SslConf* sslConf()const {
        return m_sslConf;
    }

    /*!
     * \brief token Token used for the connection.
     * \return nullptr if empty otherwise return the token obtained.
     *
     * Token will always be empty if the connection doesn't use a token.
     *
     * The token will be empty if it was not acquired yet.
     */
    const vcurl::Token* token() const {
        return m_token;
    }
    /*!
     * \brief verboseMode Configured default Verbose mode
     * \return true if verbose is active otherwise return false.
     *
     * The verbose default mode is false if not specified.
     *
     * This can be overrided at each request.
     */
    bool verboseMode() const {
        return m_verboseMode;
    }
    /*!
     * \brief noProxy No proxy configuration setting.
     * \return nullptr if the configuration is empty otherwise return the no proxy
     * configured.
     *
     * The no proxy default mode is nullptr. The system will use the system proxy
     * for all requests.
     *
     */
    const std::string* noProxy() const {
        return m_noProxy;
    }
    /*!
     * \brief timeOut Configured requests timeout
     * \return timeOut value.
     *
     * 0 value means infinite. 0 is set as default.
     */
    int timeOut() const {
        return m_timeout;
    }

    kCloudMode mode() const {
        return m_mode;
    }

    /*!
     * \brief setCurlResponseCb Overwrite the write callback function to get the result data
     * \param curlResponsesCb Callback to override the default callback
     * \param curlResponseData Userdata to provide to the callback.
     */
    void setCurlResponseCb(size_t (*curlResponsesCb)(void* contents, size_t size, size_t nmemb, void* userdata),
                           void* curlResponseData);
  protected:
    void setCurlAuthCb(size_t (*curlAuthCb)(void* contents, size_t size, size_t nmemb, void* userdata));

  private:
    vcurl::CurlSync* m_curl;
    uv_loop_t* m_loop;

    /*!
     * \brief m_hostUrl Is the connection host url.
     *
     * <STRONG>It must not finish with a trailing "/" character.</STRONG>.
     */
    std::string m_hostUrl;

    const vcurl::Lsc3Credentials* m_lsc3Cred;
    const vcurl::SslConf* m_sslConf;
    vcurl::Token* m_token;

    const std::vector<std::string>* m_customHeader;
    bool m_chunkedFile;

    bool m_verboseMode;
    /*!
     * \brief m_noProxy No proxy curl configuration
     *
     * By default, Curl use the system proxy configuration. If this variable is used, the connection can ignore
     * the system proxy configuration for those addresses.
     *
     * Valid values are:
     *   - nullptr (default) to always the system proxy.
     *   - host urls separated by a ";" character.
     *   - "*" to always ignore the system proxy.
     */
    const std::string* m_noProxy;
    /*!
     * \brief m_timeout Timeout in (ms) to set to the connection requests.
     */
    int m_timeout;

    /*!
     * \brief m_mode Connection cloud mode.
     */
    kCloudMode m_mode;

    size_t (*m_curlAuthCb)(void* contents, size_t size, size_t nmemb, void* userdata);
    size_t (*m_curlResponsesCb)(void* contents, size_t size, size_t nmemb, void* userdata);

    /*!
     * \brief m_curlResponseData User data to set to the curl write callback.
     */
    void* m_curlResponseData;

    /*!
     * \brief Default authentication callback
     * \param contents Content return by the server
     * \param size Content size.
     * \param nmemb
     * \param data User data to use.
     * \return size * nmemb
     *
     * This method store update the connection token.
     */
    static size_t curl_default_auth_write_callback(void* contents, size_t size, size_t nmemb, void* data);
    /*!
     * \brief Default callback called when a response was written by the server
     * \param contents Content return by the server
     * \param size Content size.
     * \param nmemb
     * \param data User data to use.
     * \return size * nmemb
     *
     * This method display the response content.
     *
     * <STRONG>This callback is called only if the server has written a data in its response.</STRONG> This can be use to get a
     * GET request result.
     *
     * This callback can be called many time in the same request.
     */
    static size_t curl_default_responses_write_callback(void* contents, size_t size, size_t nmemb, void* data);

    /*!
     * \brief formatRequestParams Format the request in parameters.
     * \param params Parameters to format
     * \param requestType Type of request
     * \param url Url to join
     * \param verbose Verbose mode
     * \return true
     *
     * This method will set SslConfiguration or token depending the parameters. This will configure this will format the header
     * and body request.
     *
     */
    bool formatRequestParams(vcurl::RequestParams& params,
                             vcurl::kRequestType requestType,
                             const std::string& url,
                             bool verbose);
    /*!
     * \brief formatRequestOut Format the request out parameters
     * \param out Out request parameters
     * \return true
     *
     * This will set the curl out callback.
     */
    bool formatRequestOut(vcurl::RequestOut& out);
};

} /* namespace conti_http */
} /* namespace connectivity */
} /* namespace vogon */

#endif // VOGON_CONNECTIVITY_CONTI_HTTP_HTTP_CORE_H_
