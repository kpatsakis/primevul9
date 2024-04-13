static void comma_list_body(struct buf *buf,
                            const char *vals[], unsigned flags, va_list args)
{
    const char *sep = "";
    int i;

    for (i = 0; vals[i]; i++) {
        if (flags & (1 << i)) {
            buf_appendcstr(buf, sep);
            if (args) buf_vprintf(buf, vals[i], args);
            else buf_appendcstr(buf, vals[i]);
            sep = ", ";
        }
        else if (args) {
            /* discard any unused args */
            vsnprintf(NULL, 0, vals[i], args);
        }
    }
}