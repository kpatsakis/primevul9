static void bin_read_key(conn *c, enum bin_substates next_substate, int extra) {
    assert(c);
    c->substate = next_substate;
    c->rlbytes = c->keylen + extra;

    /* Ok... do we have room for the extras and the key in the input buffer? */
    ptrdiff_t offset = c->rcurr + sizeof(protocol_binary_request_header) - c->rbuf;
    if (c->rlbytes > c->rsize - offset) {
        size_t nsize = c->rsize;
        size_t size = c->rlbytes + sizeof(protocol_binary_request_header);

        while (size > nsize) {
            nsize *= 2;
        }

        if (nsize != c->rsize) {
            if (settings.verbose > 1) {
                fprintf(stderr, "%d: Need to grow buffer from %lu to %lu\n",
                        c->sfd, (unsigned long)c->rsize, (unsigned long)nsize);
            }
            char *newm = realloc(c->rbuf, nsize);
            if (newm == NULL) {
                if (settings.verbose) {
                    fprintf(stderr, "%d: Failed to grow buffer.. closing connection\n",
                            c->sfd);
                }
                conn_set_state(c, conn_closing);
                return;
            }

            c->rbuf= newm;
            /* rcurr should point to the same offset in the packet */
            c->rcurr = c->rbuf + offset - sizeof(protocol_binary_request_header);
            c->rsize = nsize;
        }
        if (c->rbuf != c->rcurr) {
            memmove(c->rbuf, c->rcurr, c->rbytes);
            c->rcurr = c->rbuf;
            if (settings.verbose > 1) {
                fprintf(stderr, "%d: Repack input buffer\n", c->sfd);
            }
        }
    }

    /* preserve the header in the buffer.. */
    c->ritem = c->rcurr + sizeof(protocol_binary_request_header);
    conn_set_state(c, conn_nread);
}