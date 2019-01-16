/**
 * @file
 *          curlasync.cpp
 * @brief
 *          curlasync util base class
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

#include "curlasync.h"
#include "core/log.h"

#include "core/utils/base64.h"

namespace vogon {
namespace connectivity {
namespace utils {
namespace curl {

V_DECLARE_CLASS(CurlAsync)

curl_context_t* CurlAsync::create_curl_context(curl_socket_t sockfd) {
    curl_context_t* context;

    context = (curl_context_t*) malloc(sizeof * context);

    context->sockfd = sockfd;

    int r = uv_poll_init_socket(m_loop, &context->poll_handle, sockfd);
    assert(r == 0);
    context->poll_handle.data = context;
    context->cas = this;

    return context;
}

void CurlAsync::curl_close_cb(uv_handle_t* handle) {
    curl_context_t* context = (curl_context_t*) handle->data;
    if (context != NULL)
        free(context);
    context = NULL;
}

CurlAsync::CurlAsync(uv_loop_t* loop)
    : vcore::VObject() {
    VLOG_CONSTRUCTOR();

    initializeCurlAsync(loop);
}


void CurlAsync::initializeCurlAsync(uv_loop_t* loop) {
    m_multi_handle = curl_multi_init();
    curl_multi_setopt(m_multi_handle, CURLMOPT_SOCKETFUNCTION, handle_socket_cb);
    curl_multi_setopt(m_multi_handle, CURLMOPT_SOCKETDATA, this);
    curl_multi_setopt(m_multi_handle, CURLMOPT_TIMERFUNCTION, start_timeout_cb);
    curl_multi_setopt(m_multi_handle, CURLMOPT_TIMERDATA, this);
    m_handle_count = 0;
    m_timeout.data = this;
    m_loop = loop;
    m_loop->data = this;
    uv_timer_init(m_loop, &m_timeout);
}

CurlAsync::~CurlAsync() {
    curl_multi_cleanup(m_multi_handle);
}


void CurlAsync::destroyCurlContext(curl_context_t* context) {
    uv_close((uv_handle_t*) &context->poll_handle, curl_close_cb);
}


void CurlAsync::on_timeout_cb(uv_timer_t* req) {
    CurlAsync* h = (CurlAsync*)req->data;
    h->onTimeout();
}


void CurlAsync::onTimeout() {
    int running_handles;
    curl_multi_socket_action(m_multi_handle, CURL_SOCKET_TIMEOUT, 0, &running_handles);
    checkMultiInfo();
}

//OVERRIDE checkMultiInfo if you need special handling for e.g. error codes
void CurlAsync::checkMultiInfo() {
    char* done_url;
    std::string url_str;
    int response_code;
    CURLMsg* message;
    int pending;

    while ((message = curl_multi_info_read(m_multi_handle, &pending))) {
        switch (message->msg) {
            case CURLMSG_DONE:
                curl_easy_getinfo(message->easy_handle, CURLINFO_EFFECTIVE_URL,
                                  &done_url);
                url_str = done_url;

                curl_easy_getinfo(message->easy_handle, CURLINFO_RESPONSE_CODE,
                                  &response_code);

                curl_multi_remove_handle(m_multi_handle, message->easy_handle);
                curl_easy_cleanup(message->easy_handle);

                if (response_code < 300 && response_code >= 200) {
                    //curlasynclog->info("Response Code {}: {}",response_code, url_str);
                    VLOG_INFO_STATIC("CURL", "Response Code {}: {}", response_code, url_str);
                } else {
                    VLOG_ERROR_STATIC("CURL", "Operation incomplete {}: {}", response_code, url_str);
                }
                break;

            default:
                VLOG_ERROR_STATIC("CURL", "Default msg");
                abort();
        }
    }
}


void CurlAsync::curl_perform_cb(uv_poll_t* req, int status, int events) {
    curl_context_t* context = (curl_context_t*)req;

    CurlAsync* ah = context->cas;

    ah->stopTimer();
    int running_handles;
    int flags = 0;
    if (status < 0) flags = CURL_CSELECT_ERR;
    if (!status && (events & UV_READABLE)) {
        flags |= CURL_CSELECT_IN;
    }
    if (!status && (events & UV_WRITABLE)) {
        flags |= CURL_CSELECT_OUT;
    }

    curl_multi_socket_action(ah->m_multi_handle, context->sockfd, flags, &running_handles);
    ah->checkMultiInfo();
}


int CurlAsync::handle_socket_cb(CURL* easy, curl_socket_t s, int action, void* userp, void* socketp) {
    (void)easy; //not used
    CurlAsync* ah = (CurlAsync*)userp;

    curl_context_t* curl_context;
    if (action == CURL_POLL_IN || action == CURL_POLL_OUT || action == CURL_POLL_INOUT) {
        if (socketp) {
            curl_context = (curl_context_t*) socketp;
        } else {
            curl_context = ah->create_curl_context(s);
            curl_multi_assign(ah->m_multi_handle, s, (void*) curl_context);
        }
    }

    switch (action) {
        case CURL_POLL_IN:
            uv_poll_start(&curl_context->poll_handle, UV_READABLE, curl_perform_cb);
            break;
        case CURL_POLL_OUT:
            uv_poll_start(&curl_context->poll_handle, UV_WRITABLE, curl_perform_cb);
            break;
        case CURL_POLL_INOUT:
            uv_poll_start(&curl_context->poll_handle, UV_READABLE | UV_WRITABLE, curl_perform_cb);
            break;
        case CURL_POLL_REMOVE:
            if (socketp) {
                uv_poll_stop(&((curl_context_t*)socketp)->poll_handle);
                ah->destroyCurlContext((curl_context_t*) socketp);
                curl_multi_assign(ah->m_multi_handle, s, NULL);
            }
            break;
        default:
            abort();
    }

    return 0;
}

void CurlAsync::startTimer(int timeout_ms, int repeat_ms) {
    uv_timer_start(&m_timeout, on_timeout_cb, timeout_ms, repeat_ms);
}

void CurlAsync::stopTimer() {
    uv_timer_stop(&m_timeout);
}

void CurlAsync::start_timeout_cb(CURLM* multi, int timeout_ms, void* userp) {
    (void)multi; //not used
    CurlAsync* ah = (CurlAsync*)userp;
    if (timeout_ms <= 0) {
        timeout_ms = 1; /* 0 means directly call socket_action, but we'll do it in a bit */
    }
    ah->startTimer(timeout_ms, 0);
}

void CurlAsync::handleRequest(RequestParams& in,
                              RequestOut& output) {
    CURL* curl = formatRequest(in, output);
    curl_multi_add_handle(m_multi_handle, curl);
}

} /* namespace curl */
} /* namespace utils */
} /* namespace connectivity */
} /* namespace vogon */
