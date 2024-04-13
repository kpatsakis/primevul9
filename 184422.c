static void dump_UTF8_string (char *string, FILE *dst)
{
    while (*string) {
        char *p = string, *temp;
        int len = 0;

        while (*p) {
            if (*p != '\r')
                ++len;

            if (*p++ == '\n')
                break;
        }

        if (!len)
            return;

        p = temp = malloc (len * 2 + 1);

        while (*string) {
            if (*string != '\r')
                *p++ = *string;

            if (*string++ == '\n')
                break;
        }

        *p = 0;

#ifdef _WIN32
        if (!no_utf8_convert && dst != stdout && dst != stderr)
#else
        if (!no_utf8_convert)
#endif
            UTF8ToAnsi (temp, len * 2);

        fputs (temp, dst);
        free (temp);
    }
}