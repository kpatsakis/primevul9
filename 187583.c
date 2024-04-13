EXPORTED void buf_printf_markup(struct buf *buf, unsigned level,
                                const char *fmt, ...)
{
    va_list args;
    const char *eol = "\n";

    if (!config_httpprettytelemetry) {
        level = 0;
        eol = "";
    }

    va_start(args, fmt);

    buf_printf(buf, "%*s", level * MARKUP_INDENT, "");
    buf_vprintf(buf, fmt, args);
    buf_appendcstr(buf, eol);

    va_end(args);
}