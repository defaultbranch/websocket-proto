#include <stdio.h>
#include <signal.h>
#include <string.h>

#include <libwebsockets.h>


static void log_emit_function(int level, const char *line) {
    printf("%s\n", line);
}


static int callback_minimal(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {

    struct per_session_data *session_data = (struct per_session_data*) user;

    switch(reason) {

        case LWS_CALLBACK_PROTOCOL_INIT:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_PROTOCOL_INIT) not implemented for %p\n", __FILE__, __LINE__, wsi);
            break;

        case LWS_CALLBACK_EVENT_WAIT_CANCELLED:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_EVENT_WAIT_CANCELLED) not implemented for %p\n", __FILE__, __LINE__, wsi);
            break;

        default:
            printf("%s(%d) callback_minimal(reason: %d) not implemented\n", __FILE__, __LINE__, reason);
    }

    return 0;
}


int main(int argc, char **argv) {

    // configure logging
    lws_set_log_level(LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE, log_emit_function);
    lwsl_user("client started");

    // specify protocols
    struct lws_protocols protocols[] = {
            { "lws-minimal-proxy", callback_minimal, 0, 0 },
            { NULL, NULL, 0, 0 } /* terminator */
    };

    // specify context
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof info);
    info.port = CONTEXT_PORT_NO_LISTEN;  // client does not listen
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