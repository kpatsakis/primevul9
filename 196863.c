static void process_memlimit_command(conn *c, token_t *tokens, const size_t ntokens) {
    uint32_t memlimit;
    assert(c != NULL);

    set_noreply_maybe(c, tokens, ntokens);

    if (!safe_strtoul(tokens[1].value, &memlimit)) {
        out_string(c, "ERROR");
    } else {
        if (memlimit < 8) {
            out_string(c, "MEMLIMIT_TOO_SMALL cannot set maxbytes to less than 8m");
        } else {
            if (slabs_adjust_mem_limit((size_t) memlimit * 1024 * 1024)) {
                if (settings.verbose > 0) {
                    fprintf(stderr, "maxbytes adjusted to %llum\n", (unsigned long long)memlimit);
                }

                out_string(c, "OK");
            } else {
                out_string(c, "MEMLIMIT_ADJUST_FAILED out of bounds or unable to adjust");
            }
        }
    }
}