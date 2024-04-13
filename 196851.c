static void process_update_command(conn *c, token_t *tokens, const size_t ntokens, int comm, bool handle_cas) {
    char *key;
    size_t nkey;
    unsigned int flags;
    int32_t exptime_int = 0;
    time_t exptime;
    int vlen;
    uint64_t req_cas_id=0;
    item *it;

    assert(c != NULL);

    set_noreply_maybe(c, tokens, ntokens);

    if (tokens[KEY_TOKEN].length > KEY_MAX_LENGTH) {
        out_string(c, "CLIENT_ERROR bad command line format");
        return;
    }

    key = tokens[KEY_TOKEN].value;
    nkey = tokens[KEY_TOKEN].length;

    if (! (safe_strtoul(tokens[2].value, (uint32_t *)&flags)
           && safe_strtol(tokens[3].value, &exptime_int)
           && safe_strtol(tokens[4].value, (int32_t *)&vlen))) {
        out_string(c, "CLIENT_ERROR bad command line format");
        return;
    }

    /* Ubuntu 8.04 breaks when I pass exptime to safe_strtol */
    exptime = exptime_int;

    /* Negative exptimes can underflow and end up immortal. realtime() will
       immediately expire values that are greater than REALTIME_MAXDELTA, but less
       than process_started, so lets aim for that. */
    if (exptime < 0)
        exptime = REALTIME_MAXDELTA + 1;

    // does cas value exist?
    if (handle_cas) {
        if (!safe_strtoull(tokens[5].value, &req_cas_id)) {
            out_string(c, "CLIENT_ERROR bad command line format");
            return;
        }
    }

    if (vlen < 0 || vlen > (INT_MAX - 2)) {
        out_string(c, "CLIENT_ERROR bad command line format");
        return;
    }
    vlen += 2;

    if (settings.detail_enabled) {
        stats_prefix_record_set(key, nkey);
    }

    it = item_alloc(key, nkey, flags, realtime(exptime), vlen);

    if (it == 0) {
        enum store_item_type status;
        if (! item_size_ok(nkey, flags, vlen)) {
            out_string(c, "SERVER_ERROR object too large for cache");
            status = TOO_LARGE;
        } else {
            out_of_memory(c, "SERVER_ERROR out of memory storing object");
            status = NO_MEMORY;
        }
        LOGGER_LOG(c->thread->l, LOG_MUTATIONS, LOGGER_ITEM_STORE,
                NULL, status, comm, key, nkey, 0, 0);
        /* swallow the data line */
        c->write_and_go = conn_swallow;
        c->sbytes = vlen;

        /* Avoid stale data persisting in cache because we failed alloc.
         * Unacceptable for SET. Anywhere else too? */
        if (comm == NREAD_SET) {
            it = item_get(key, nkey, c, DONT_UPDATE);
            if (it) {
                item_unlink(it);
                item_remove(it);
            }
        }

        return;
    }
    ITEM_set_cas(it, req_cas_id);

    c->item = it;
    c->ritem = ITEM_data(it);
    c->rlbytes = it->nbytes;
    c->cmd = comm;
    conn_set_state(c, conn_nread);
}