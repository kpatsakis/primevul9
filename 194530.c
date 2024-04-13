j2k_error(const char *msg, void *client_data) {
    JPEG2KDECODESTATE *state = (JPEG2KDECODESTATE *)client_data;
    free((void *)state->error_msg);
    state->error_msg = strdup(msg);
}