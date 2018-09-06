#include <stdio.h>
#include "sqlite3.h"
#include "duktape.h"
#include "mongoose.h"
#include "curl/curl.h"

#include "mz.h"
#include "mz_os.h"
#include "mz_strm.h"
#include "mz_strm_mem.h"
#include "mz_zip.h"
/*
#include "mysql.h"

void show_error(MYSQL *mysql)
{
    printf("Error(%d) [%s] \"%s\"", mysql_errno(mysql),
           mysql_sqlstate(mysql),
           mysql_error(mysql));
    mysql_close(mysql);
    exit(-1);
}*/

static size_t _curl_write_callback(
    char *buffer,
    size_t size,
    size_t nitems,
    void *outstream)
{
    printf("%.*s\n", size * nitems, buffer);
    return size * nitems;
}

static size_t _curl_write_header_callback(
    char *buffer,
    size_t size,
    size_t nitems,
    void *outstream)
{
    printf("%.*s\n", size * nitems, buffer);
    return size * nitems;
}

static const char *s_http_port = "8443";
static const char *s_ssl_cert = "choice.crt";
static const char *s_ssl_key = "choice.key";
static struct mg_serve_http_opts s_http_server_opts;

static void ev_handler(struct mg_connection *nc, int ev, void *p)
{
    if (ev == MG_EV_HTTP_REQUEST)
    {
        mg_serve_http(nc, (struct http_message *)p, s_http_server_opts);
    }
}

void *mg_thread(void *param)
{
    struct mg_mgr mgr;
    struct mg_connection *nc;
    struct mg_bind_opts bind_opts;
    const char *err;

    mg_mgr_init(&mgr, NULL);
    memset(&bind_opts, 0, sizeof(bind_opts));
    bind_opts.ssl_cert = s_ssl_cert;
    bind_opts.ssl_key = s_ssl_key;
    bind_opts.error_string = &err;

    printf("Starting SSL server on port %s, cert from %s, key from %s\n",
           s_http_port, bind_opts.ssl_cert, bind_opts.ssl_key);
    nc = mg_bind_opt(&mgr, s_http_port, ev_handler, bind_opts);
    if (nc == NULL)
    {
        printf("Failed to create listener: %s\n", err);
        return (void *)1;
    }

    // Set up HTTP server parameters
    mg_set_protocol_http_websocket(nc);
    s_http_server_opts.document_root = "."; // Serve current directory
    s_http_server_opts.enable_directory_listing = "yes";

    for (;;)
    {
        mg_mgr_poll(&mgr, 1000);
    }
    mg_mgr_free(&mgr);
    return 0;
}

//target_link_libraries(uv advapi32 iphlpapi psapi userenv shell32 ws2_32)
int main(int argc, char const *argv[])
{
    printf("hello world\n");
    sqlite3 *sqlite = 0;
    int ret = sqlite3_open("test.db", &sqlite);
    if (!ret)
    {
        sqlite3_close(sqlite);
        printf("sqlite3 load success,version: %s\n", sqlite3_libversion());
    }
    else
    {
        printf("duktape load failed!\n");
    }
    duk_context *ctx = duk_create_heap_default();
    if (ctx)
    {
        duk_destroy_heap(ctx);
        printf("duktape load success!\n");
    }
    else
    {
        printf("duktape load failed!\n");
    }
    printf("libcurl version:%s\n", curl_version());

    mg_start_thread(mg_thread, 0);
    {
        CURL *curl = curl_easy_init();
        CURLcode curlCode = curl_easy_setopt(curl, CURLOPT_URL, "http://nboh.choicesaas.cn/settled.html#/merchants/register?_k=aia2sw");
        printf("curlCode=%d\n", curlCode);
        curlCode = curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");
        printf("curlCode=%d\n", curlCode);
        curlCode = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_callback);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, _curl_write_header_callback);
        printf("curlCode=%d\n", curlCode);
        curlCode = curl_easy_perform(curl);
        printf("curlCode=%d\n", curlCode);
        long code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
        curl_easy_cleanup(curl);
        printf("baidu resp code=%d\n", code);
    }
    {
        CURL *curl = curl_easy_init();
        CURLcode curlCode = curl_easy_setopt(curl, CURLOPT_URL, "https://www.baidu.com");
        printf("curlCode=%d\n", curlCode);
        curlCode = curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "gzip");
        printf("curlCode=%d\n", curlCode);
        curlCode = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_callback);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, _curl_write_header_callback);
#ifdef WIN32
        curl_easy_setopt(curl, CURLOPT_CAINFO, "D:\\code\\appsrv\\trunk\\ca-bundle.crt");
#else
        curl_easy_setopt(curl, CURLOPT_CAINFO, "../../ca-bundle.crt");
#endif
        printf("curlCode=%d\n", curlCode);
        curlCode = curl_easy_perform(curl);
        printf("curlCode=%d\n", curlCode);
        long code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
        curl_easy_cleanup(curl);
        printf("baidu resp code=%d\n", code);
    }
    {
        CURL *curl = curl_easy_init();
        CURLcode curlCode = curl_easy_setopt(curl, CURLOPT_URL, "https://127.0.0.1:8443");
        printf("curlCode=%d\n", curlCode);
        curlCode = curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1);
        curlCode = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1);
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, _curl_write_callback);
        curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, _curl_write_header_callback);
#ifdef WIN32
        curl_easy_setopt(curl, CURLOPT_CAINFO, "choice-rootCA.crt");
#else
        curl_easy_setopt(curl, CURLOPT_CAINFO, "../../ca-bundle.crt");
#endif
        printf("curlCode=%d\n", curlCode);
        curlCode = curl_easy_perform(curl);
        printf("curlCode=%d\n", curlCode);
        long code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
        curl_easy_cleanup(curl);
        printf("baidu resp code=%d\n", code);
    }
    //Sleep(3000);

    char output[256];
    int32_t ok = 0;

    memset(output, 'z', sizeof(output));
    mz_path_resolve("c:\\test\\.", output, sizeof(output));
    ok = (strcmp(output, "c:\\test\\") == 0);
    /*
    {
        MYSQL *mysql;
        const char *query;

        mysql_debug("d:t:O");

        mysql = mysql_init(NULL);

        if (!mysql_real_connect(mysql, "192.168.218.1", "root", "ipconfig",
                                "test", 3306, 0, 0))
            show_error(mysql);

        query = "DROP TABLE IF EXISTS debug_example";
        if (mysql_real_query(mysql, query, strlen(query)))
            show_error(mysql);

        query = "CREATE TABLE debug_example (id int not null, my_name varchar(50),"
                "PRIMARY KEY(id))";
        if (mysql_real_query(mysql, query, strlen(query)))
            show_error(mysql);

        mysql_close(mysql);
    }
    */
    system("pause");
    return 0;
}
