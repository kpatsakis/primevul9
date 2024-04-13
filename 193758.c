pick_ipc_buffer(int max)
{
    const char *env = getenv("PCMK_ipc_buffer");

    if (env) {
        max = crm_parse_int(env, "0");
    }

    if (max <= 0) {
        max = MAX_MSG_SIZE;
    }

    if (max < MIN_MSG_SIZE) {
        max = MIN_MSG_SIZE;
    }

    crm_trace("Using max message size of %d", max);
    return max;
}