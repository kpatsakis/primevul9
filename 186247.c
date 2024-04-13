static void complete_incr_bin(conn *c) {
    item *it;
    char *key;
    size_t nkey;
    /* Weird magic in add_delta forces me to pad here */
    char tmpbuf[INCR_MAX_STORAGE_LEN];
    uint64_t cas = 0;

    protocol_binary_response_incr* rsp = (protocol_binary_response_incr*)c->wbuf;
    protocol_binary_request_incr* req = binary_get_request(c);

    assert(c != NULL);
    assert(c->wsize >= sizeof(*rsp));

    /* fix byteorder in the request */
    req->message.body.delta = ntohll(req->message.body.delta);
    req->message.body.initial = ntohll(req->message.body.initial);
    req->message.body.expiration = ntohl(req->message.body.expiration);
    key = binary_get_key(c);
    nkey = c->binary_header.request.keylen;

    if (settings.verbose > 1) {
        int i;
        fprintf(stderr, "incr ");

        for (i = 0; i < nkey; i++) {
            fprintf(stderr, "%c", key[i]);
        }
        fprintf(stderr, " %lld, %llu, %d\n",
                (long long)req->message.body.delta,
                (long long)req->message.body.initial,
                req->message.body.expiration);
    }

    if (c->binary_header.request.cas != 0) {
        cas = c->binary_header.request.cas;
    }
    switch(add_delta(c, key, nkey, c->cmd == PROTOCOL_BINARY_CMD_INCREMENT,
                     req->message.body.delta, tmpbuf,
                     &cas)) {
    case OK:
        rsp->message.body.value = htonll(strtoull(tmpbuf, NULL, 10));
        if (cas) {
            c->cas = cas;
        }
        write_bin_response(c, &rsp->message.body, 0, 0,
                           sizeof(rsp->message.body.value));
        break;
    case NON_NUMERIC:
        write_bin_error(c, PROTOCOL_BINARY_RESPONSE_DELTA_BADVAL, 0);
        break;
    case EOM:
        write_bin_error(c, PROTOCOL_BINARY_RESPONSE_ENOMEM, 0);
        break;
    case DELTA_ITEM_NOT_FOUND:
        if (req->message.body.expiration != 0xffffffff) {
            /* Save some room for the response */
            rsp->message.body.value = htonll(req->message.body.initial);
            it = item_alloc(key, nkey, 0, realtime(req->message.body.expiration),
                            INCR_MAX_STORAGE_LEN);

            if (it != NULL) {
                snprintf(ITEM_data(it), INCR_MAX_STORAGE_LEN, "%llu",
                         (unsigned long long)req->message.body.initial);

                if (store_item(it, NREAD_ADD, c)) {
                    c->cas = ITEM_get_cas(it);
                    write_bin_response(c, &rsp->message.body, 0, 0, sizeof(rsp->message.body.value));
                } else {
                    write_bin_error(c, PROTOCOL_BINARY_RESPONSE_NOT_STORED, 0);
                }
                item_remove(it);         /* release our reference */
            } else {
                write_bin_error(c, PROTOCOL_BINARY_RESPONSE_ENOMEM, 0);
            }
        } else {
            pthread_mutex_lock(&c->thread->stats.mutex);
            if (c->cmd == PROTOCOL_BINARY_CMD_INCREMENT) {
                c->thread->stats.incr_misses++;
            } else {
                c->thread->stats.decr_misses++;
            }
            pthread_mutex_unlock(&c->thread->stats.mutex);

            write_bin_error(c, PROTOCOL_BINARY_RESPONSE_KEY_ENOENT, 0);
        }
        break;
    case DELTA_ITEM_CAS_MISMATCH:
        write_bin_error(c, PROTOCOL_BINARY_RESPONSE_KEY_EEXISTS, 0);
        break;
    }
}