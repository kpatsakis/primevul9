static int ws_echo(struct buf *inbuf, struct buf *outbuf,
                   struct buf *logbuf __attribute__((unused)),
                   void **rock __attribute__((unused)))
{
    buf_init_ro(outbuf, buf_base(inbuf), buf_len(inbuf));

    return 0;
}