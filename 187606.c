HIDDEN void log_cachehdr(const char *name, const char *contents,
                         const char *raw, void *rock)
{
    struct buf *buf = (struct buf *) rock;

    /* Ignore private headers in our cache */
    if (name[0] == ':') return;

    if (!strcasecmp(name, "authorization")) {
        /* Replace authorization credentials with an ellipsis */
        const char *creds = strchr(contents, ' ') + 1;
        buf_printf(buf, "%c%s: %.*s%-*s\r\n", toupper(name[0]), name+1,
                   (int) (creds - contents), contents,
                   (int) strlen(creds), "...");
    }
    else if (raw)
        buf_appendcstr(buf, raw);
    else
        buf_printf(buf, "%c%s: %s\r\n", toupper(name[0]), name+1, contents);
}