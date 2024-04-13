va_log(struct usbredirparser_priv *parser, int verbose, const char *fmt, ...)
{
    char buf[512];
    va_list ap;
    int n;

    n = sprintf(buf, "usbredirparser: ");
    va_start(ap, fmt);
    vsnprintf(buf + n, sizeof(buf) - n, fmt, ap);
    va_end(ap);

    parser->callb.log_func(parser->callb.priv, verbose, buf);
}