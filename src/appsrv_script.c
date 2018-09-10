#include "appsrv_script.h"
#include "appsrv_log.h"
#include "jsbind/duk_console.h"
#include "jsbind/duk_module_duktape.h"

static void fatal_function(void *udata, const char *msg)
{
    printf("error info: %s\n", msg);
}

duk_context *appsrv_script_create()
{
    duk_context *ctx = duk_create_heap(0, 0, 0, 0, fatal_function);
    duk_console_init(ctx, DUK_CONSOLE_STDOUT_ONLY | DUK_CONSOLE_STDERR_ONLY);
    duk_module_duktape_init(ctx);
    return ctx;
}

int appsrv_script_run(duk_context *ctx, const char *script, char **out)
{
    int ret = duk_peval_string(ctx, script);
    const char *ret_val = duk_safe_to_string(ctx, -1);
    if (ret_val)
        *out = strdup(ret_val);
    return ret;
}

void appsrv_script_destroy(duk_context *ctx)
{
    if (ctx)
        duk_destroy_heap(ctx);
}