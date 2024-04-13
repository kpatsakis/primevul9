EXPORTED const char *http_statusline(unsigned ver, long code)
{
    static struct buf statline = BUF_INITIALIZER;

    if (ver == VER_2) buf_setcstr(&statline, HTTP2_VERSION);
    else {
        buf_setmap(&statline, HTTP_VERSION, HTTP_VERSION_LEN-1);
        buf_putc(&statline, ver + '0');
    }

    buf_putc(&statline, ' ');
    buf_appendcstr(&statline, error_message(code));
    return buf_cstring(&statline);
}