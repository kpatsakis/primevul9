static void process_bin_get_or_touch(conn *c) {
    item *it;

    protocol_binary_response_get* rsp = (protocol_binary_response_get*)c->wbuf;
    char* key = binary_get_key(c);
    size_t nkey = c->binary_header.request.keylen;
    int should_touch = (c->cmd == PROTOCOL_BINARY_CMD_TOUCH ||
                        c->cmd == PROTOCOL_BINARY_CMD_GAT ||
                        c->cmd == PROTOCOL_BINARY_CMD_GATK);
    int should_return_key = (c->cmd == PROTOCOL_BINARY_CMD_GETK ||
                             c->cmd == PROTOCOL_BINARY_CMD_GATK);
    int should_return_value = (c->cmd != PROTOCOL_BINARY_CMD_TOUCH);

    if (settings.verbose > 1) {
        fprintf(stderr, "<%d %s ", c->sfd, should_touch ? "TOUCH" : "GET");
        if (fwrite(key, 1, nkey, stderr)) {}
        fputc('\n', stderr);
    }

    if (should_touch) {
        protocol_binary_request_touch *t = binary_get_request(c);
        time_t exptime = ntohl(t->message.body.expiration);

        it = item_touch(key, nkey, realtime(exptime), c);
    } else {
        it = item_get(key, nkey, c, DO_UPDATE);
    }

    if (it) {
        /* the length has two unnecessary bytes ("\r\n") */
        uint16_t keylen = 0;
        uint32_t bodylen = sizeof(rsp->message.body) + (it->nbytes - 2);

        pthread_mutex_lock(&c->thread->stats.mutex);
        if (should_touch) {
            c->thread->stats.touch_cmds++;
            c->thread->stats.slab_stats[ITEM_clsid(it)].touch_hits++;
        } else {
            c->thread->stats.get_cmds++;
            c->thread->stats.slab_stats[ITEM_clsid(it)].get_hits++;
        }
        pthread_mutex_unlock(&c->thread->stats.mutex);

        if (should_touch) {
            MEMCACHED_COMMAND_TOUCH(c->sfd, ITEM_key(it), it->nkey,
                                    it->nbytes, ITEM_get_cas(it));
        } else {
            MEMCACHED_COMMAND_GET(c->sfd, ITEM_key(it), it->nkey,
                                  it->nbytes, ITEM_get_cas(it));
        }

        if (c->cmd == PROTOCOL_BINARY_CMD_TOUCH) {
            bodylen -= it->nbytes - 2;
        } else if (should_return_key) {
            bodylen += nkey;
            keylen = nkey;
        }

        add_bin_header(c, 0, sizeof(rsp->message.body), keylen, bodylen);
        rsp->message.header.response.cas = htonll(ITEM_get_cas(it));

        // add the flags
        if (settings.inline_ascii_response) {
            rsp->message.body.flags = htonl(strtoul(ITEM_suffix(it), NULL, 10));
        } else {
            rsp->message.body.flags = htonl(*((uint32_t *)ITEM_suffix(it)));
        }
        add_iov(c, &rsp->message.body, sizeof(rsp->message.body));

        if (should_return_key) {
            add_iov(c, ITEM_key(it), nkey);
        }

        if (should_return_value) {
            /* Add the data minus the CRLF */
            if ((it->it_flags & ITEM_CHUNKED) == 0) {
                add_iov(c, ITEM_data(it), it->nbytes - 2);
            } else {
                add_chunked_item_iovs(c, it, it->nbytes - 2);
            }
        }

        conn_set_state(c, conn_mwrite);
        c->write_and_go = conn_new_cmd;
        /* Remember this command so we can garbage collect it later */
        c->item = it;
    } else {
        pthread_mutex_lock(&c->thread->stats.mutex);
        if (should_touch) {
            c->thread->stats.touch_cmds++;
            c->thread->stats.touch_misses++;
        } else {
            c->thread->stats.get_cmds++;
            c->thread->stats.get_misses++;
        }
        pthread_mutex_unlock(&c->thread->stats.mutex);

        if (should_touch) {
            MEMCACHED_COMMAND_TOUCH(c->sfd, key, nkey, -1, 0);
        } else {
            MEMCACHED_COMMAND_GET(c->sfd, key, nkey, -1, 0);
        }

        if (c->noreply) {
            conn_set_state(c, conn_new_cmd);
        } else {
            if (should_return_key) {
                char *ofs = c->wbuf + sizeof(protocol_binary_response_header);
                add_bin_header(c, PROTOCOL_BINARY_RESPONSE_KEY_ENOENT,
                        0, nkey, nkey);
                memcpy(ofs, key, nkey);
                add_iov(c, ofs, nkey);
                conn_set_state(c, conn_mwrite);
                c->write_and_go = conn_new_cmd;
            } else {
                write_bin_error(c, PROTOCOL_BINARY_RESPONSE_KEY_ENOENT,
                                NULL, 0);
            }
        }
    }

    if (settings.detail_enabled) {
        stats_prefix_record_get(key, nkey, NULL != it);
    }
}