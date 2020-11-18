#include <stdio.h>
#include <signal.h>
#include <string.h>

#include <libwebsockets.h>


static void log_emit_function(int level, const char *line) {
    printf("%s\n", line);
}


int main(int argc, char **argv) {

    // configure logging
    lws_set_log_level(LLL_USER | LLL_ERR | LLL_WARN | LLL_NOTICE, log_emit_function);
    lwsl_user("client started");

    // specify context
    struct lws_context_creation_info info;
    memset(&info, 0, sizeof info);
    info.port = CONTEXT_PORT_NO_LISTEN;  // client does not listen
    info.foreign_loops = NULL;  // can point to sd_loop later

    // create context
    struct lws_context *context;

    context = lws_create_context(&info);
    if (!context) {
        lwsl_err("lws init failed\n");
        return 1;
    }

    return 0;
}