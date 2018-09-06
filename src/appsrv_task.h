#ifndef APPSRV_TASK_H_
#define APPSRV_TASK_H_

typedef struct
{
} appsrv_task_container;

int appsrv_task_container_init(
    const char *container_name,
    int persistence,
    int occurrences,
    appsrv_task_container *task_container);

#endif