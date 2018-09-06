#ifndef APPSRV_COMMON_H_
#define APPSRV_COMMON_H_

typedef struct
{
    char *data;
    unsigned int size;
} appsrv_data;

void appsrv_free(void *buf);

#endif