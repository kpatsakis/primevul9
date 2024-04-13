int usbredirparser_unserialize(struct usbredirparser *parser_pub,
                               uint8_t *state, int len)
{
    struct usbredirparser_priv *parser =
        (struct usbredirparser_priv *)parser_pub;
    struct usbredirparser_buf *wbuf, **next;
    uint32_t orig_caps[USB_REDIR_CAPS_SIZE];
    uint8_t *data;
    uint32_t i, l, header_len, remain = len;

    if (unserialize_int(parser, &state, &remain, &i, "magic"))
        return -1;
    if (i != USBREDIRPARSER_SERIALIZE_MAGIC) {
        ERROR("error unserialize magic mismatch");
        return -1;
    }

    if (unserialize_int(parser, &state, &remain, &i, "length"))
        return -1;
    if (i != len) {
        ERROR("error unserialize length mismatch");
        return -1;
    }

    data = (uint8_t *)parser->our_caps;
    i = USB_REDIR_CAPS_SIZE * sizeof(int32_t);
    memcpy(orig_caps, parser->our_caps, i);
    if (unserialize_data(parser, &state, &remain, &data, &i, "our_caps"))
        return -1;
    for (i =0; i < USB_REDIR_CAPS_SIZE; i++) {
        if (parser->our_caps[i] != orig_caps[i]) {
            /* orig_caps is our original settings
             * parser->our_caps is off the wire.
             * We want to allow reception from an older
             * usbredir that doesn't have all our features.
             */
            if (parser->our_caps[i] & ~orig_caps[i]) {
                /* Source has a cap we don't */
                ERROR("error unserialize caps mismatch ours: %x recv: %x",
                      orig_caps[i], parser->our_caps[i]);
                return -1;
            } else {
                /* We've got a cap the source doesn't - that's OK */
                WARNING("unserialize missing some caps; ours: %x recv: %x",
                      orig_caps[i], parser->our_caps[i]);
            }
        }
    }

    data = (uint8_t *)parser->peer_caps;
    i = USB_REDIR_CAPS_SIZE * sizeof(int32_t);
    if (unserialize_data(parser, &state, &remain, &data, &i, "peer_caps"))
        return -1;
    if (i)
        parser->have_peer_caps = 1;

    if (unserialize_int(parser, &state, &remain, &i, "skip"))
        return -1;
    parser->to_skip = i;

    header_len = usbredirparser_get_header_len(parser_pub);
    data = (uint8_t *)&parser->header;
    i = header_len;
    if (unserialize_data(parser, &state, &remain, &data, &i, "header"))
        return -1;
    parser->header_read = i;

    /* Set various length field froms the header (if we've a header) */
    if (parser->header_read == header_len) {
                int type_header_len =
                    usbredirparser_get_type_header_len(parser_pub,
                                                       parser->header.type, 0);
                if (type_header_len < 0 ||
                    type_header_len > sizeof(parser->type_header) ||
                    parser->header.length < type_header_len ||
                    (parser->header.length > type_header_len &&
                     !usbredirparser_expect_extra_data(parser))) {
                    ERROR("error unserialize packet header invalid");
                    return -1;
                }
                parser->type_header_len = type_header_len;
                parser->data_len = parser->header.length - type_header_len;
    }

    data = parser->type_header;
    i = parser->type_header_len;
    if (unserialize_data(parser, &state, &remain, &data, &i, "type_header"))
        return -1;
    parser->type_header_read = i;

    if (parser->data_len) {
        parser->data = malloc(parser->data_len);
        if (!parser->data) {
            ERROR("Out of memory allocating unserialize buffer");
            return -1;
        }
    }
    i = parser->data_len;
    if (unserialize_data(parser, &state, &remain, &parser->data, &i, "data"))
        return -1;
    parser->data_read = i;

    /* Get the write buffer count and the write buffers */
    if (unserialize_int(parser, &state, &remain, &i, "write_buf_count"))
        return -1;
    next = &parser->write_buf;
    while (i) {
        wbuf = calloc(1, sizeof(*wbuf));
        if (!wbuf) {
            ERROR("Out of memory allocating unserialize buffer");
            return -1;
        }
        *next = wbuf;
        l = 0;
        if (unserialize_data(parser, &state, &remain, &wbuf->buf, &l, "wbuf"))
            return -1;
        wbuf->len = l;
        next = &wbuf->next;
        i--;
    }

    if (remain) {
        ERROR("error unserialize %d bytes of extraneous state data", remain);
        return -1;
    }

    return 0;
}