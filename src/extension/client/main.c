#include "shared/protocol.h"
#include "shared/network.h"
#include "client/network.h"
#include "client/state.h"
#include "client/game.h"
#include "shared/log.h"
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>

/*
Entry point for client executable
Initialises local state storage and runs the game loop
*/

int main(int argc, char **argv) {
    // Setup logging - TO BE IMPROVED
    if (!init_logger(NULL)) {
        return EXIT_FAILURE;
    }

    ClientState state = init_client_state();
    state.is_running = true;

    char *hostname;
    if (argc == 2) {
        hostname = argv[1];
    } else {
        hostname = "127.0.0.1";
    }

    int port = 8080;

    LOG_INFO("Client starting up. Target host: %s", hostname);

    if (start_client_systems(&state, hostname, port)) {
        client_loop(&state);
    }

    client_cleanup(&state);
    close_logger();

    return 0;
}
