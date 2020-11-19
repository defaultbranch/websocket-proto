#include <stdio.h>
#include <signal.h>
#include <string.h>

#include <libwebsockets.h>


static struct my_conn {
    lws_sorted_usec_list_t	sul;	     /* schedule connection retry */
    struct lws		*wsi;	     /* related wsi if any */
    uint16_t		retry_count; /* count of consequetive retries */
} mco;


static void log_emit_function(int level, const char *line) {
    printf("%s\n", line);
}


static struct lws_context *context;


static const uint32_t backoff_ms[] = { 1000, 2000, 3000, 4000, 5000 };

static const lws_retry_bo_t retry = {
        .retry_ms_table			= backoff_ms,
        .retry_ms_table_count		= LWS_ARRAY_SIZE(backoff_ms),
        .conceal_count			= LWS_ARRAY_SIZE(backoff_ms),

        .secs_since_valid_ping		= 3,  /* force PINGs after secs idle */
        .secs_since_valid_hangup	= 10, /* hangup after secs idle */

        .jitter_percent			= 20,
};

static void
connect_client(lws_sorted_usec_list_t *sul)
{
    printf("%s(%d) connect_client started\n", __FILE__, __LINE__);

    struct my_conn *mco = lws_container_of(sul, struct my_conn, sul);

    struct lws_client_connect_info i;
    memset(&i, 0, sizeof(i));

    i.context = context;
    i.port = 8080;
    i.address = "localhost";
    i.path = "/";
    i.host = i.address;
    i.origin = i.address;
    i.ssl_connection = (enum lws_client_connect_ssl_connection_flags) 0;
    i.protocol = "lws-minimal-proxy";
    i.local_protocol_name = "lws-minimal-client";
    i.pwsi = &mco->wsi;
    i.retry_and_idle_policy = &retry;
    i.userdata = mco;

    if (!lws_client_connect_via_info(&i))
        /*
         * Failed... schedule a retry... we can't use the _retry_wsi()
         * convenience wrapper api here because no valid wsi at this
         * point.
         */
        if (lws_retry_sul_schedule(context, 0, sul, &retry,
                                   connect_client, &mco->retry_count)) {
            lwsl_err("%s: connection attempts exhausted\n", __func__);
        }
}

static int sendCounter = 0;
static int recvCounter = 0;
static char* sendBuffer = NULL;

static int callback_minimal(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {

    struct per_session_data *session_data = (struct per_session_data*) user;

    switch(reason) {

        case LWS_CALLBACK_PROTOCOL_INIT:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_PROTOCOL_INIT) not implemented for %p\n", __FILE__, __LINE__, wsi);
            break;

        case LWS_CALLBACK_EVENT_WAIT_CANCELLED:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_EVENT_WAIT_CANCELLED) not implemented for %p\n", __FILE__, __LINE__, wsi);
            break;

        case LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_SERVER_NEW_CLIENT_INSTANTIATED) not implemented for %p\n", __FILE__, __LINE__, wsi);
            break;

        case LWS_CALLBACK_WSI_CREATE:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_WSI_CREATE) not implemented for %p\n", __FILE__, __LINE__, wsi);
            break;

        case LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_CLIENT_APPEND_HANDSHAKE_HEADER) not implemented for %p\n", __FILE__, __LINE__, wsi);
            break;

        case LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_ESTABLISHED_CLIENT_HTTP) not implemented for %p\n", __FILE__, __LINE__, wsi);
            break;

        case LWS_CALLBACK_CLIENT_WRITEABLE:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_CLIENT_WRITEABLE) sending for %p\n", __FILE__, __LINE__, wsi);

            // free previous send buffer
            free(sendBuffer);

            // create and fill new send buffer
            pid_t pid = getpid();
            char *template = "Client %d send %d recv %d\n";
            size_t msgLen = snprintf(NULL, 0, template, pid, sendCounter, recvCounter) + 1;
            sendBuffer = malloc(LWS_PRE + msgLen);
            snprintf(sendBuffer+LWS_PRE, msgLen, template, pid, sendCounter, recvCounter);

            lws_write(wsi, ((unsigned char*)sendBuffer)+LWS_PRE, msgLen, LWS_WRITE_TEXT);
            ++sendCounter;
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_CLIENT_RECEIVE) for %p with length %d\n", __FILE__, __LINE__, wsi, len);
            printf("%s\n", in);
            ++recvCounter;
            break;

        case LWS_CALLBACK_CLIENT_RECEIVE_PONG:
            printf("%s(%d) callback_minimal(reason: LWS_CALLBACK_CLIENT_RECEIVE_PONG) not implemented for %p\n", __FILE__, __LINE__, wsi);
            break;

        case 2:
        case 3:
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
    context = lws_create_context(&info);
    if (!context) {
        lwsl_err("lws init failed\n");
        return 1;
    }

    lws_sul_schedule(context, 0, &mco.sul, connect_client, 1);

    // run
    int n=0;
    while (n >= 0) {
        n = lws_service(context, 0);
    }

    // cleanup
    lws_context_destroy(context);

    return 0;
}