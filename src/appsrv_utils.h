#ifndef APPSRV_UTILS_H_
#define APPSRV_UTILS_H_

#include <mongoose.h>

#define appsrv_vsnprintf mg_avprintf

typedef struct
{
    char *data;
    unsigned int size;
} appsrv_data;

#endif
