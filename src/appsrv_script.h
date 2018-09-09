#ifndef APPSRV_SCRIPT_H_
#define APPSRV_SCRIPT_H_
#include <duktape.h>

duk_context *appsrv_script_create();

int appsrv_script_run(duk_context *ctx, const char *script);

void appsrv_script_destroy(duk_context *ctx);

#endif