static void serialize_test(struct usbredirparser *parser_pub)
{
    struct usbredirparser_priv *parser =
        (struct usbredirparser_priv *)parser_pub;
    struct usbredirparser_buf *wbuf, *next_wbuf;
    uint8_t *data;
    int len;

    if (usbredirparser_serialize(parser_pub, &data, &len))
        return;

    wbuf = parser->write_buf;
    while (wbuf) {
        next_wbuf = wbuf->next;
        free(wbuf->buf);
        free(wbuf);
        wbuf = next_wbuf;
    }
    parser->write_buf = NULL;
    parser->write_buf_count = 0;

    free(parser->data);
    parser->data = NULL;

    parser->type_header_len = parser->data_len = parser->have_peer_caps = 0;

    usbredirparser_unserialize(parser_pub, data, len);
    free(data);
}