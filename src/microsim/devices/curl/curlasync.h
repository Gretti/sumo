/*******************************************************************************
 * Copyright (C) Continental Automotive GmbH 2018
 * Alle Rechte vorbehalten. All Rights Reserved.
 * The reproduction, transmission or use of this document or its contents is not
 * permitted without express written authority. Offenders will be liable for
 * damages. All rights, including rights created by patent grant or registration
 * of a utility model or design, are reserved.
 ******************************************************************************/
#ifndef VOGON_CONNECTIVITY_UTILS_CURL_CURLASYNC_H_
#define VOGON_CONNECTIVITY_UTILS_CURL_CURLASYNC_H_

/**
 * @file
 *          curlasync.h
 * @brief
 *          curlasync util base class header
 *
 * @par Author (last changes):
 *          - Laurent Bousquet
 *          - Phone: +33 (0)6 37 85 83 21
 *          - Copyright (C) Continental Automotive GmbH 2018
 * @par Responsible Architect:
 *          - Laurent Bousquet
 *          - Copyright (C) Continental Automotive GmbH 2018
 * @par Platforms:
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
 *  Not tested yet
 *
 * @par Description:
 *
 *
 * @par Module-History:
 * @verbatim
 *  Date        Author             Reason
 *  06.13.2018   Laurent Bousquet  Imported in Vogon
 *  11.04.2017   Virgil Fenzl   Initial Draft
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
#include "connectivity/utils/curl/curlcommon.h"
#include "connectivity/utils/curl/curl.h"

#include <uv.h>

#include "core/vobject.h"

#include <assert.h>

namespace vogon {
namespace connectivity {
namespace utils {
namespace curl {

class CurlAsync;

typedef struct curl_context_s {
    uv_poll_t poll_handle;
    curl_socket_t sockfd;
    CurlAsync* cas;
} curl_context_t;

class CurlAsync: public vcore::VObject {

  public:
    explicit CurlAsync(uv_loop_t* loop);
    virtual ~CurlAsync();

    void handleRequest(RequestParams &in,
                       RequestOut& output);

    const std::string &className()const;

  protected:
    uv_loop_t *m_loop;
    CURLM *m_multi_handle;

  private:
    uv_timer_t m_timeout;

    int m_handle_count;

    // uv callbacks
    static void on_timeout_cb(uv_timer_t *req);
    static void curl_close_cb(uv_handle_t *handle);

    // curl callbacks
    static void start_timeout_cb(CURLM *multi, int timeout_ms, void *userp);
    static int handle_socket_cb(CURL *easy, curl_socket_t s, int action, void *userp, void *socketp);
    static void curl_perform_cb(uv_poll_t *req, int status, int events);

    // private instance operations
    curl_context_t *create_curl_context(curl_socket_t sockfd);
    void destroyCurlContext(curl_context_t *context);
    void checkMultiInfo();
    void onTimeout();

    void startTimer(int timeout, int repeat);
    void stopTimer();

    void initializeCurlAsync(uv_loop_t* loop);
};

} /* namespace curl */
} /* namespace utils */
} /* namespace connectivity */
} /* namespace vogon */

#endif // VOGON_CONNECTIVITY_UTILS_CURL_CURLASYNC_H_
