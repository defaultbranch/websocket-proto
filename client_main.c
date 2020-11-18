#include <stdio.h>
#include <signal.h>
#include <string.h>

#include <libwebsockets.h>


#define LWS_PLUGIN_STATIC
#if !defined (LWS_PLUGIN_STATIC)
#define LWS_DLL
#define LWS_INTERNAL
#include <libwebsockets.h>
#endif

int main(int argc, char **argv) {
    printf("Client started\n");
    return 0;
}