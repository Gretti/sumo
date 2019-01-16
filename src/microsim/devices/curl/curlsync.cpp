/**
 * @file
 *          curlsync.cpp
 * @brief
 *          curlsync util base class
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
 *
 *
 * @par Description:
 *
 *  This is the curl common class to format the right curl request
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

#include "curlsync.h"
#include "core/log.h"

namespace vogon {
namespace connectivity {
namespace utils {
namespace curl {

V_DECLARE_CLASS(CurlSync)

CurlSync::CurlSync()
    : vcore::VObject() {
    VLOG_CONSTRUCTOR();
    curl_global_init(CURL_GLOBAL_ALL);
}

CurlSync::~CurlSync() {
    curl_global_cleanup();
}

void CurlSync::handleRequest(RequestParams& in,
                             RequestOut& output) {
    CURL* curl = formatRequest(in, output);

    output.result = curl_easy_perform(curl);
    curl_easy_cleanup(curl);
}

} /* namespace curl */
} /* namespace utils */
} /* namespace connectivity */
} /* namespace vogon */
