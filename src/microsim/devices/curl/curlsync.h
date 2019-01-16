/*******************************************************************************
 * Copyright (C) Continental Automotive GmbH 2018
 * Alle Rechte vorbehalten. All Rights Reserved.
 * The reproduction, transmission or use of this document or its contents is not
 * permitted without express written authority. Offenders will be liable for
 * damages. All rights, including rights created by patent grant or registration
 * of a utility model or design, are reserved.
 ******************************************************************************/
#ifndef VOGON_CONNECTIVITY_UTILS_CURL_CURLSYNC_H_
#define VOGON_CONNECTIVITY_UTILS_CURL_CURLSYNC_H_

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
 *  06.13.2018   Laurent Bousquet  Imported in Vogon
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

#include <uv.h>

#include "core/vobject.h"
#include "connectivity/utils/curl/curl.h"
#include "connectivity/utils/curl/curlcommon.h"

#include <assert.h>

namespace vogon {
namespace connectivity {
namespace utils {
namespace curl {

class CurlSync: public vcore::VObject {
  public:
    CurlSync();
    virtual ~CurlSync();

    void handleRequest(RequestParams &in,
                       RequestOut& output);

    const std::string &className()const;

  private:
};

} /* namespace curl */
} /* namespace utils */
} /* namespace connectivity */
} /* namespace vogon */

#endif // VOGON_CONNECTIVITY_UTILS_CURL_CURLSYNC_H_
