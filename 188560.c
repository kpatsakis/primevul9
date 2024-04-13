static void ga_osrelease_replace_special(gchar *value)
{
    gchar *p, *p2, quote;

    /* Trim the string at first space or semicolon if it is not enclosed in
     * single or double quotes. */
    if ((value[0] != '"') || (value[0] == '\'')) {
        p = strchr(value, ' ');
        if (p != NULL) {
            *p = 0;
        }
        p = strchr(value, ';');
        if (p != NULL) {
            *p = 0;
        }
        return;
    }

    quote = value[0];
    p2 = value;
    p = value + 1;
    while (*p != 0) {
        if (*p == '\\') {
            p++;
            switch (*p) {
            case '$':
            case '\'':
            case '"':
            case '\\':
            case '`':
                break;
            default:
                /* Keep literal backslash followed by whatever is there */
                p--;
                break;
            }
        } else if (*p == quote) {
            *p2 = 0;
            break;
        }
        *(p2++) = *(p++);
    }
}