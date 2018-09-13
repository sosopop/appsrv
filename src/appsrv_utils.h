#ifndef APPSRV_UTILS_H_
#define APPSRV_UTILS_H_

#include <mongoose.h>
#include <mz_os.h>

#define appsrv_vsnprintf mg_avprintf
#define appsrv_rand mz_os_rand
#define appsrv_to_hex cs_to_hex

#endif
