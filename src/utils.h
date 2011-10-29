#ifndef HTTP_BOOTER_UTILS_H
#define HTTP_BOOTER_UTILS_H


#include <vector>
#include "string.h"
#include "common.h"

// #define HB_UTILS_NS_START namespace utils {
// #define HB_UTILS_NS_END }
#define HB_UTILS_NS_START
#define HB_UTILS_NS_END

HB_NS_START
HB_UTILS_NS_START

extern String & unescape(String & string);

HB_UTILS_NS_END
HB_NS_END

#endif

