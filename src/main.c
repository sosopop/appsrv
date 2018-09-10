#include <stdio.h>
#include <assert.h>
#include "appsrv.h"
#ifdef WIN32
#define _CRTDBG_MAP_ALLOC
#include <windows.h>
#include <stdlib.h>
#include <crtdbg.h>
#endif

extern int test();
extern int test1();

//target_link_libraries(uv advapi32 iphlpapi psapi userenv shell32 ws2_32)
int main(int argc, char const *argv[])
{
#ifdef WIN32
    _CrtSetDbgFlag(_CrtSetDbgFlag(_CRTDBG_REPORT_FLAG) | _CRTDBG_LEAK_CHECK_DF);
#endif
	assert(test() == APPSRV_E_OK);
	assert(test1() == APPSRV_E_OK);
#if defined(_MSC_VER) || defined(__MINGW64_VERSION_MAJOR)
    assert(_CrtDumpMemoryLeaks() == 0);
#endif
    return 0;
}
