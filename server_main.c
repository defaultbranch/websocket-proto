#include <stdio.h>
#include <signal.h>
#include <string.h>

#include <libwebsockets.h>


static void log_emit_function(int level, const char *line) {
    printf("%s\n", line);
}


struct per_session_data {
    struct per_session_data *pss_list;
    struct lws *wsi;
    uint32_t tail;
};


static int callback_minimal(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {

    struct per_session_data *session_data = (struct per_session_data*) user;
    printf("%s(%d) callback_minimal(reason: %d) not implemented\n", __FILE__, __LINE__, reason);
}

int main(int argc, char **argv) {

    // configure logging
    lws_set_log_level(LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE, log_emit_function);
    lwsl_user("server started");

    // specify protocols
    struct lws_protocols protocols[] = {
            { "http", lws_callback_http_dummy, 0, 0 },
            { "lws-minimal-proxy", callback_minimal, sizeof(struct per_session_data), 128, 0, NULL, 0 },
            { NULL, NULL, 0, 0 } /* terminator */
    };

    // specify context
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof info);
    info.port = 8080;  // server listens to http port
    info.protocols = protocols;
    info.foreign_loops = NULL;  // can point to sd_loop later

    // create context
    struct lws_context *context;
    context = lws_create_context(&info);
    if (!context) {
        lwsl_err("lws init failed\n");
        return 1;
    }

    // run
    int n=0;
    while (n >= 0) {
        n = lws_service(context, 0);
    }

    // cleanup
    lws_context_destroy(context);

	return 0;
}
