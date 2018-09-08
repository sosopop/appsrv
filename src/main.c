#include <stdio.h>
#include "appsrv.h"

//target_link_libraries(uv advapi32 iphlpapi psapi userenv shell32 ws2_32)
int main(int argc, char const *argv[])
{
    appsrv_handle app = appsrv_create();

cleanup:
    appsrv_wait_and_close(app);
    return 0;
}
