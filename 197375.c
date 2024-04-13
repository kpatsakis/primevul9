void usbredirparser_destroy(struct usbredirparser *parser_pub)
{
    struct usbredirparser_priv *parser =
        (struct usbredirparser_priv *)parser_pub;
    struct usbredirparser_buf *wbuf, *next_wbuf;

    free(parser->data);
    parser->data = NULL;

    wbuf = parser->write_buf;
    while (wbuf) {
        next_wbuf = wbuf->next;
        free(wbuf->buf);
        free(wbuf);
        wbuf = next_wbuf;
    }

    if (parser->lock)
        parser->callb.free_lock_func(parser->lock);

    free(parser);
}