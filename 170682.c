static char *_unescape_lf(char *str)
{
    char *c, *p = str;
    gsize len = 0;

    while ((c = strchr(p, '\\')) != NULL)
    {
        if (p != &str[len])
            memmove(&str[len], p, c - p);
        len += (c - p);
        if (c[1] == 'n')
        {
            str[len++] = '\n';
            c++;
        }
        else if (c != &str[len])
            str[len++] = *c;
        p = &c[1];
    }
    if (p != &str[len])
        memmove(&str[len], p, strlen(p) + 1);
    return str;
}