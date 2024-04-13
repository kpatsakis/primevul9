static void trace_cachehdr(const char *name, const char *contents, const char *raw, void *rock)
{
    struct buf *buf = (struct buf *) rock;
    const char **hdr, *sensitive[] =
        { "authorization", "cookie", "proxy-authorization", NULL };

    /* Ignore private headers in our cache */
    if (name[0] == ':') return;

    for (hdr = sensitive; *hdr && strcmp(name, *hdr); hdr++);

    if (!*hdr) {
        if (raw) buf_appendcstr(buf, raw);
        else buf_printf(buf, "%c%s: %s\r\n",
                            toupper(name[0]), name+1, contents);
    }
}