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


static char* sendBuffer = NULL;

static int callback_minimal(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {

    struct per_session_data *session_data = (struct per_session_data*) user;

    switch (reason) {

        case LWS_CALLBACK_PROTOCOL_INIT:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_PROTOCOL_INIT) not implemented for %p\n", __FILE__, __LINE__, wsi);
            break;

        case LWS_CALLBACK_EVENT_WAIT_CANCELLED:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_EVENT_WAIT_CANCELLED) not implemented for %p\n", __FILE__, __LINE__, wsi);
            break;

        case LWS_CALLBACK_HTTP_BIND_PROTOCOL:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_HTTP_BIND_PROTOCOL) not implemented for %p\n", __FILE__, __LINE__, wsi);
            break;

        case LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_FILTER_PROTOCOL_CONNECTION) not implemented for %p\n", __FILE__, __LINE__, wsi);
            break;

        case LWS_CALLBACK_ADD_HEADERS:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_ADD_HEADERS) not implemented for %p\n", __FILE__, __LINE__, wsi);
            break;

        case LWS_CALLBACK_SERVER_WRITEABLE:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_SERVER_WRITEABLE) not implemented for %p\n", __FILE__, __LINE__, wsi);

            char *msg = "Blubber di Blub\n";
            size_t msgLen = strlen(msg) + 1;

            // free previous send buffer
            free(sendBuffer);

            // create and fill new send buffer
            sendBuffer = malloc(LWS_PRE + msgLen);
            strcpy(sendBuffer+LWS_PRE, msg);

            lws_write(wsi, ((unsigned char*)sendBuffer)+LWS_PRE, msgLen, LWS_WRITE_TEXT);
            break;

        case LWS_CALLBACK_RECEIVE:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_RECEIVE) for %p with length %d\n", __FILE__, __LINE__, wsi, len);
            lwsl_hexdump_notice(in, len);
            break;

        case LWS_CALLBACK_WS_SERVER_DROP_PROTOCOL:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_WS_SERVER_DROP_PROTOCOL) not implemented for %p\n", __FILE__, __LINE__, wsi);
            break;

        case 0:
        case 4:
        default:
            printf("%s(%d) callback_minimal(reason: %d) not implemented\n", __FILE__, __LINE__, reason);
    }

    return 0;
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
