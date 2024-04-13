int usbredirparser_do_write(struct usbredirparser *parser_pub)
{
    struct usbredirparser_priv *parser =
        (struct usbredirparser_priv *)parser_pub;
    struct usbredirparser_buf* wbuf;
    int w, ret = 0;

    LOCK(parser);
    for (;;) {
        wbuf = parser->write_buf;
        if (!wbuf)
            break;

        w = wbuf->len - wbuf->pos;
        w = parser->callb.write_func(parser->callb.priv,
                                     wbuf->buf + wbuf->pos, w);
        if (w <= 0) {
            ret = w;
            break;
        }

        /* See usbredirparser_write documentation */
        if ((parser->flags & usbredirparser_fl_write_cb_owns_buffer) &&
                w != wbuf->len)
            abort();

        wbuf->pos += w;
        if (wbuf->pos == wbuf->len) {
            parser->write_buf = wbuf->next;
            if (!(parser->flags & usbredirparser_fl_write_cb_owns_buffer))
                free(wbuf->buf);
            free(wbuf);
            parser->write_buf_count--;
        }
    }
    UNLOCK(parser);
    return ret;
}