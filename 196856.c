static void process_bin_sasl_auth(conn *c) {
    // Guard for handling disabled SASL on the server.
    if (!settings.sasl) {
        write_bin_error(c, PROTOCOL_BINARY_RESPONSE_UNKNOWN_COMMAND, NULL,
                        c->binary_header.request.bodylen
                        - c->binary_header.request.keylen);
        return;
    }

    assert(c->binary_header.request.extlen == 0);

    int nkey = c->binary_header.request.keylen;
    int vlen = c->binary_header.request.bodylen - nkey;

    if (nkey > MAX_SASL_MECH_LEN) {
        write_bin_error(c, PROTOCOL_BINARY_RESPONSE_EINVAL, NULL, vlen);
        c->write_and_go = conn_swallow;
        return;
    }

    char *key = binary_get_key(c);
    assert(key);

    item *it = item_alloc(key, nkey, 0, 0, vlen+2);

    /* Can't use a chunked item for SASL authentication. */
    if (it == 0 || (it->it_flags & ITEM_CHUNKED)) {
        write_bin_error(c, PROTOCOL_BINARY_RESPONSE_ENOMEM, NULL, vlen);
        c->write_and_go = conn_swallow;
        return;
    }

    c->item = it;
    c->ritem = ITEM_data(it);
    c->rlbytes = vlen;
    conn_set_state(c, conn_nread);
    c->substate = bin_reading_sasl_auth_data;
}