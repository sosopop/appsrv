cmake_minimum_required(VERSION 3.0.0)
project(appsrv LANGUAGES C VERSION 0.1.0)

include(CheckIncludeFile)

if(WIN32)
    if ("${CMAKE_GENERATOR}" MATCHES "Visual Studio ([0-9]+)")
        # Note: "v110_xp" is for Visual Studio 11 2012, which is unsupported.
        if (${CMAKE_MATCH_1} EQUAL 12)
            # Visual Studio 12 2013
            set(CMAKE_GENERATOR_TOOLSET "v120_xp" CACHE STRING "CMAKE_GENERATOR_TOOLSET" FORCE)
            set(CMAKE_VS_PLATFORM_TOOLSET "v120_xp" CACHE STRING "CMAKE_VS_PLATFORM_TOOLSET" FORCE)
        elseif (${CMAKE_MATCH_1} EQUAL 14)
            # Visual Studio 14 2015
            set(CMAKE_GENERATOR_TOOLSET "v140_xp" CACHE STRING "CMAKE_GENERATOR_TOOLSET" FORCE)
            set(CMAKE_VS_PLATFORM_TOOLSET "v140_xp" CACHE STRING "CMAKE_VS_PLATFORM_TOOLSET" FORCE)
        elseif (${CMAKE_MATCH_1} EQUAL 15)
            # Visual Studio 15 2017
            set(CMAKE_GENERATOR_TOOLSET "v141_xp" CACHE STRING "CMAKE_GENERATOR_TOOLSET" FORCE)
            set(CMAKE_VS_PLATFORM_TOOLSET "v141_xp" CACHE STRING "CMAKE_VS_PLATFORM_TOOLSET" FORCE)
        endif()
    endif()
    
    set(CompilerFlags
            CMAKE_CXX_FLAGS
            CMAKE_CXX_FLAGS_DEBUG
            CMAKE_CXX_FLAGS_RELEASE
            CMAKE_C_FLAGS
            CMAKE_C_FLAGS_DEBUG
            CMAKE_C_FLAGS_RELEASE
            )
    foreach(CompilerFlag ${CompilerFlags})
      string(REPLACE "/MD" "/MT" ${CompilerFlag} "${${CompilerFlag}}")
    endforeach()
    
    set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} /MT")
    set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} /MTd")

    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /SUBSYSTEM:CONSOLE,5.01")

    add_definitions(-DBUILD_WINDOWS -D_WIN32_WINNT=0x0501 -D_CRT_NONSTDC_NO_DEPRECATE -D_CRT_SECURE_NO_WARNINGS)
endif()

if(UNIX)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -O3")
endif()

add_definitions(-DFORCE_LOWQUALITY_ENTROPY)

add_subdirectory(3rdparty/zlib)
add_subdirectory(3rdparty/sqlite3)
add_subdirectory(3rdparty/duktape)
add_subdirectory(3rdparty/mongoose)

#set(WITH_SIGNCODE OFF CACHE BOOL "" FORCE)
#set(WITH_RTC OFF CACHE BOOL "" FORCE)
#set(WITH_EXTERNAL_ZLIB ON CACHE BOOL "" FORCE)
#set(WITH_OPENSSL OFF CACHE BOOL "" FORCE)
#set(WITH_SQLITE OFF CACHE BOOL "" FORCE)
#set(CLIENT_DOCS "" CACHE STRING "" FORCE)
#add_subdirectory(3rdparty/mariadb-connector-c)

set(USE_BZIP2 OFF CACHE BOOL "" FORCE)
set(USE_LZMA OFF CACHE BOOL "" FORCE)
set(USE_PKCRYPT OFF CACHE BOOL "" FORCE)
set(BUILD_TEST OFF CACHE BOOL "" FORCE)
set(ZLIB_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/zlib" CACHE STRING "" FORCE)
add_subdirectory(3rdparty/minizip)

set(ENABLE_ZLIB_SUPPORT ON CACHE BOOL "" FORCE)
set(ENABLE_PROGRAMS OFF CACHE BOOL "" FORCE)
set(ENABLE_TESTING OFF CACHE BOOL "" FORCE)

add_subdirectory(3rdparty/mbedtls)

set(HTTP_ONLY ON CACHE BOOL "" FORCE)
set(CURL_STATICLIB ON CACHE BOOL "" FORCE)
set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
set(ENABLE_INET_PTON OFF CACHE BOOL "" FORCE)

set(CMAKE_USE_MBEDTLS ON CACHE BOOL "" FORCE)
set(MBEDTLS_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/mbedtls/include" CACHE STRING "" FORCE)
#设置可信证书
#set(CURL_CA_BUNDLE "${CMAKE_CURRENT_SOURCE_DIR}/ca-bundle.crt" CACHE STRING "" FORCE)

if(UNIX)
    set(CMAKE_USE_OPENSSL OFF CACHE BOOL "" FORCE)
    set(CMAKE_USE_LIBSSH2 OFF CACHE BOOL "" FORCE)
endif()

if(WIN32)
    set(CMAKE_USE_WINSSL OFF CACHE BOOL "" FORCE)
elseif(ANDROID)
    set(CMAKE_USE_OPENSSL OFF CACHE BOOL "" FORCE)
    set(CMAKE_USE_LIBSSH2 OFF CACHE BOOL "" FORCE)

    set(HAVE_POLL_FINE_EXITCODE "0" CACHE STRING "Result from TRY_RUN" FORCE)
    set(HAVE_POLL_FINE_EXITCODE__TRYRUN_OUTPUT "" CACHE STRING "Output from TRY_RUN" FORCE)
endif()

add_subdirectory(3rdparty/libcurl)
add_subdirectory(src)

add_dependencies(libcurl zlibstatic mbedcrypto mbedtls mbedx509)
add_dependencies(mongoose zlibstatic mbedcrypto mbedtls mbedx509)
add_dependencies(libminizip zlibstatic)