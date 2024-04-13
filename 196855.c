static void process_bin_delete(conn *c) {
    item *it;

    protocol_binary_request_delete* req = binary_get_request(c);

    char* key = binary_get_key(c);
    size_t nkey = c->binary_header.request.keylen;

    assert(c != NULL);

    if (settings.verbose > 1) {
        int ii;
        fprintf(stderr, "Deleting ");
        for (ii = 0; ii < nkey; ++ii) {
            fprintf(stderr, "%c", key[ii]);
        }
        fprintf(stderr, "\n");
    }

    if (settings.detail_enabled) {
        stats_prefix_record_delete(key, nkey);
    }

    it = item_get(key, nkey, c, DONT_UPDATE);
    if (it) {
        uint64_t cas = ntohll(req->message.header.request.cas);
        if (cas == 0 || cas == ITEM_get_cas(it)) {
            MEMCACHED_COMMAND_DELETE(c->sfd, ITEM_key(it), it->nkey);
            pthread_mutex_lock(&c->thread->stats.mutex);
            c->thread->stats.slab_stats[ITEM_clsid(it)].delete_hits++;
            pthread_mutex_unlock(&c->thread->stats.mutex);
            item_unlink(it);
            write_bin_response(c, NULL, 0, 0, 0);
        } else {
            write_bin_error(c, PROTOCOL_BINARY_RESPONSE_KEY_EEXISTS, NULL, 0);
        }
        item_remove(it);      /* release our reference */
    } else {
        write_bin_error(c, PROTOCOL_BINARY_RESPONSE_KEY_ENOENT, NULL, 0);
        pthread_mutex_lock(&c->thread->stats.mutex);
        c->thread->stats.delete_misses++;
        pthread_mutex_unlock(&c->thread->stats.mutex);
    }
}