cmake_minimum_required(VERSION 3.0.0)

include_directories("../3rdparty/sqlite3")
include_directories("../3rdparty/duktape")
include_directories("../3rdparty/libcurl/include")
include_directories("../3rdparty/libcurl/lib")
include_directories("../3rdparty/zlib")
include_directories("../3rdparty/mongoose")
include_directories("../3rdparty/mbedtls/include")
include_directories("../3rdparty/minizip")
#include_directories("../3rdparty/mariadb-connector-c/include")

add_definitions(-DCURL_STATICLIB -DAPPSRV_ENABLE_DEBUG -DMG_ENABLE_SSL -DMG_ENABLE_MQTT_BROKER)

aux_source_directory(. APPSRV_SOURCE)

add_executable(appsrv ${APPSRV_SOURCE})

if( NOT APPLE AND NOT WIN32 )
    set(WHOLE_ARCHIVE_BEGIN "-Wl,--whole-archive")
    set(WHOLE_ARCHIVE_END "-Wl,--no-whole-archive")
else()
    set(WHOLE_ARCHIVE_BEGIN "")
    set(WHOLE_ARCHIVE_END "")
endif()

list(APPEND LINK_LIB sqlite3 duktape libcurl 
${WHOLE_ARCHIVE_BEGIN}
zlibstatic 
mbedcrypto mbedtls mbedx509 
${WHOLE_ARCHIVE_END}
mongoose libminizip)
if(WIN32)
    list(APPEND LINK_LIB ws2_32 iphlpapi psapi userenv)
endif()

if(UNIX)
    list(APPEND LINK_LIB pthread dl m)
endif()

if(ANDROID)
    list(REMOVE_ITEM LINK_LIB pthread)
endif()

target_link_libraries(appsrv ${LINK_LIB})