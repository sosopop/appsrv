#include "appsrv.h"
#include "appsrv_internal.h"
#include "appsrv_utils.h"
#include "appsrv_thread.h"
#include "appsrv_log.h"
#include "appsrv_script.h"

static int appsrv_create_data(struct appsrv_s *appsrv)
{
    int ret = APPSRV_E_OK;
    int sql_ret = SQLITE_OK;
    char random_data[32];
    appsrv_rand(random_data, sizeof(random_data));
    char random_hex[65] = {0};
    appsrv_to_hex(random_hex, random_data, sizeof(random_data));
    char rand_db[128] = {0};
    snprintf(rand_db, sizeof(rand_db), "file:%s?mode=memory&cache=shared", random_hex);
    appsrv->data_db_name = strdup(rand_db);
    sql_ret = sqlite3_open(rand_db, &appsrv->data_db);
    if (sql_ret != SQLITE_OK)
    {
        ret = APPSRV_E_DB_ERROR;
        goto cleanup;
    }
    sql_ret = sqlite3_busy_timeout(appsrv->data_db, SQLITE_TIMEOUT);
    if (SQLITE_OK != sql_ret)
    {
        appsrv_log(APPSRV_LOG_ERROR, "database failed: %s", sqlite3_errmsg(appsrv->data_db));
        ret = APPSRV_E_DB_ERROR;
        goto cleanup;
    }
    sql_ret = sqlite3_exec(appsrv->data_db,
                           "CREATE TABLE [__global_data] ("
                           "[key] CHAR(128), "
                           "[value] BLOB, "
                           "CONSTRAINT [] PRIMARY KEY ([key]));",
                           0, 0, 0);
    if (sql_ret != SQLITE_OK)
    {
        appsrv_log(APPSRV_LOG_ERROR, "database failed: %s", sqlite3_errmsg(appsrv->data_db));
        ret = APPSRV_E_DB_ERROR;
        goto cleanup;
    }
cleanup:
    return ret;
}

static int appsrv_destory_data(struct appsrv_s *appsrv)
{
    int ret = APPSRV_E_OK;
    if (appsrv->data_db_name)
        free(appsrv->data_db_name);
    if (appsrv->data_db)
        sqlite3_close(appsrv->data_db);
cleanup:
    return ret;
}

int appsrv_data_set(
    appsrv_handle appsrv,
    const char *key,
    struct appsrv_data *data)
{
    int ret = APPSRV_E_OK;
    sqlite3 *data_db = 0;
    struct appsrv_s *app = (struct appsrv_s *)appsrv;
    int sql_ret = sqlite3_open(
        app->data_db_name, &data_db);
    if (sql_ret != SQLITE_OK)
    {
        ret = APPSRV_E_DB_ERROR;
        goto cleanup;
    }
    sql_ret = sqlite3_busy_timeout(data_db, SQLITE_TIMEOUT);
    if (SQLITE_OK != sql_ret)
    {
        appsrv_log(APPSRV_LOG_ERROR, "database failed: %s", sqlite3_errmsg(data_db));
        ret = APPSRV_E_DB_ERROR;
        goto cleanup;
    }

    const char *query = "replace into __global_data (value, key) values ( ?, ? )";
    sqlite3_stmt *stmt = 0;
    sql_ret = sqlite3_prepare_v2(data_db, query, -1, &stmt, 0);
    if (SQLITE_OK != sql_ret)
    {
        appsrv_log(APPSRV_LOG_ERROR, "database failed: %s", sqlite3_errmsg(data_db));
        ret = APPSRV_E_DB_ERROR;
        goto cleanup;
    }
    sql_ret = sqlite3_bind_blob(stmt, 1, data->data, data->size, 0);
    if (SQLITE_OK != sql_ret)
    {
        appsrv_log(APPSRV_LOG_ERROR, "database failed: %s", sqlite3_errmsg(data_db));
        ret = APPSRV_E_DB_ERROR;
        goto cleanup;
    }
    sql_ret = sqlite3_bind_text(stmt, 2, key, -1, 0);
    if (SQLITE_OK != sql_ret)
    {
        appsrv_log(APPSRV_LOG_ERROR, "database failed: %s", sqlite3_errmsg(data_db));
        ret = APPSRV_E_DB_ERROR;
        goto cleanup;
    }
    if (sqlite3_step(stmt) != SQLITE_DONE)
    {
        appsrv_log(APPSRV_LOG_ERROR, "database failed: %s", sqlite3_errmsg(data_db));
        ret = APPSRV_E_DB_ERROR;
        goto cleanup;
    }
cleanup:
    if (stmt)
        sqlite3_finalize(stmt);
    if (data_db)
        sqlite3_close(data_db);
    return ret;
}

int appsrv_data_get(
    appsrv_handle appsrv,
    const char *key,
    struct appsrv_data *data)
{
    int ret = APPSRV_E_OK;
    sqlite3 *data_db = 0;
    struct appsrv_s *app = (struct appsrv_s *)appsrv;
    int sql_ret = sqlite3_open(
        app->data_db_name, &data_db);
    if (sql_ret != SQLITE_OK)
    {
        ret = APPSRV_E_DB_ERROR;
        goto cleanup;
    }
    sql_ret = sqlite3_busy_timeout(data_db, SQLITE_TIMEOUT);
    if (SQLITE_OK != sql_ret)
    {
        appsrv_log(APPSRV_LOG_ERROR, "database failed: %s", sqlite3_errmsg(data_db));
        ret = APPSRV_E_DB_ERROR;
        goto cleanup;
    }

    const char *query = "select value from __global_data where key = ?";
    sqlite3_stmt *stmt = 0;
    sql_ret = sqlite3_prepare_v2(data_db, query, -1, &stmt, 0);
    if (SQLITE_OK != sql_ret)
    {
        appsrv_log(APPSRV_LOG_ERROR, "database failed: %s", sqlite3_errmsg(data_db));
        ret = APPSRV_E_DB_ERROR;
        goto cleanup;
    }
    sql_ret = sqlite3_bind_text(stmt, 1, key, -1, 0);
    if (SQLITE_OK != sql_ret)
    {
        appsrv_log(APPSRV_LOG_ERROR, "database failed: %s", sqlite3_errmsg(data_db));
        ret = APPSRV_E_DB_ERROR;
        goto cleanup;
    }
    if (sqlite3_step(stmt) != SQLITE_ROW)
    {
        appsrv_log(APPSRV_LOG_ERROR, "database failed: %s", sqlite3_errmsg(data_db));
        ret = APPSRV_E_DB_ERROR;
        goto cleanup;
    }
    data->size = sqlite3_column_bytes(stmt, 0);
    if (data->size)
        data->data = strdup(sqlite3_column_blob(stmt, 0));
cleanup:
    if (stmt)
        sqlite3_finalize(stmt);
    if (data_db)
        sqlite3_close(data_db);
    return ret;
}

int appsrv_create(
    const char *script_path,
    const char *data_path,
    appsrv_handle *appsrv)
{
    int ret = APPSRV_E_OK;
    struct appsrv_s *app = 0;
    app = (struct appsrv_s *)malloc(sizeof(struct appsrv_s));
    memset(app, 0, sizeof(struct appsrv_s));
    app->data_path = strdup(data_path);
    app->script_path = strdup(script_path);

    ret = appsrv_create_data(app);
    if (ret != APPSRV_E_OK)
    {
        goto cleanup;
    }
    *appsrv = app;
cleanup:
    if (ret != APPSRV_E_OK)
    {
        appsrv_destroy(appsrv);
    }
    return ret;
}

int appsrv_post_task(
    appsrv_handle appsrv,
    const char *task_container_name,
    unsigned int delay,
    const char *script)
{
    int ret = APPSRV_E_OK;
    duk_context *script_ctx = appsrv_script_create();
    if (!script_ctx)
    {
        ret = APPSRV_E_SCRIPT_CREATE_FAILED;
        goto cleanup;
    }
    if (appsrv_script_run(script_ctx, script) != 0)
    {
        ret = APPSRV_E_SCRIPT_RUN_FAILED;
        goto cleanup;
    }
cleanup:
    if (script_ctx)
    {
        appsrv_script_destroy(script_ctx);
    }
    return ret;
}

int appsrv_wait(
    appsrv_handle appsrv)
{
    struct appsrv_s *app = (struct appsrv_s *)appsrv;
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_wait");
    return APPSRV_E_OK;
}

int appsrv_stop(
    appsrv_handle appsrv)
{
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_stop");
    struct appsrv_s *app = (struct appsrv_s *)appsrv;
    app->stop_signal = 1;
    return APPSRV_E_OK;
}

void appsrv_destroy(
    appsrv_handle appsrv)
{
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_destroy");
    struct appsrv_s *app = (struct appsrv_s *)appsrv;

    appsrv_destory_data(app);
    if (app->data_path)
        free(app->data_path);
    if (app->script_path)
        free(app->script_path);
    free(app);
}

void appsrv_free(
    void *data)
{
    appsrv_log(APPSRV_LOG_INFO, "%s", "appsrv_free");
    free(data);
}

#define APPSRV_ERROR_GEN(n, s) {"APPSRV_E_" #n, s},
static struct
{
    const char *name;
    const char *description;
} appsrv_strerror_tab[] = {APPSRV_ERROR_MAP(APPSRV_ERROR_GEN)};
#undef APPSRV_ERROR_GEN

const char *appsrv_errno_description(
    appsrv_errno error)
{
    return appsrv_strerror_tab[error].description;
}

const char *appsrv_version()
{
    return APPSRV_VERSION;
}
