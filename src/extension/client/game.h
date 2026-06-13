#include "state.h"

extern bool start_client_systems(ClientState *state, char* hostname);

extern void client_loop(ClientState *state);

extern void client_cleanup(ClientState *state);
