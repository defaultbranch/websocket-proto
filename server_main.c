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
    lwsl_user("server started");

	return 0;
}
