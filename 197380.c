static void usbredirparser_queue(struct usbredirparser *parser_pub,
    uint32_t type, uint64_t id, void *type_header_in,
    uint8_t *data_in, int data_len)
{
    struct usbredirparser_priv *parser =
        (struct usbredirparser_priv *)parser_pub;
    uint8_t *buf, *type_header_out, *data_out;
    struct usb_redir_header *header;
    struct usbredirparser_buf *wbuf, *new_wbuf;
    int header_len, type_header_len;

    header_len = usbredirparser_get_header_len(parser_pub);
    type_header_len = usbredirparser_get_type_header_len(parser_pub, type, 1);
    if (type_header_len < 0) { /* This should never happen */
        ERROR("error packet type unknown with internal call, please report!!");
        return;
    }

    if (!usbredirparser_verify_type_header(parser_pub, type, type_header_in,
                                           data_in, data_len, 1)) {
        ERROR("error usbredirparser_send_* call invalid params, please report!!");
        return;
    }

    new_wbuf = calloc(1, sizeof(*new_wbuf));
    buf = malloc(header_len + type_header_len + data_len);
    if (!new_wbuf || !buf) {
        ERROR("Out of memory allocating buffer to send packet, dropping!");
        free(new_wbuf); free(buf);
        return;
    }

    new_wbuf->buf = buf;
    new_wbuf->len = header_len + type_header_len + data_len;

    header = (struct usb_redir_header *)buf;
    type_header_out = buf + header_len;
    data_out = type_header_out + type_header_len;

    header->type   = type;
    header->length = type_header_len + data_len;
    if (usbredirparser_using_32bits_ids(parser_pub))
        ((struct usb_redir_header_32bit_id *)header)->id = id;
    else
        header->id = id;
    memcpy(type_header_out, type_header_in, type_header_len);
    memcpy(data_out, data_in, data_len);

    LOCK(parser);
    if (!parser->write_buf) {
        parser->write_buf = new_wbuf;
    } else {
        /* limiting the write_buf's stack depth is our users responsibility */
        wbuf = parser->write_buf;
        while (wbuf->next)
            wbuf = wbuf->next;

        wbuf->next = new_wbuf;
    }
    parser->write_buf_count++;
    UNLOCK(parser);
}