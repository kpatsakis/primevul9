static void UTF8ToAnsi (char *string, int len)
{
    int max_chars = (int) strlen (string);
#if defined (_WIN32)
    wchar_t *temp = malloc ((max_chars + 1) * 2);
    int act_chars = UTF8ToWideChar (string, temp);

    while (act_chars) {
        memset (string, 0, len);

        if (WideCharToMultiByte (CP_ACP, 0, temp, act_chars, string, len - 1, NULL, NULL))
            break;
        else
            act_chars--;
    }

    if (!act_chars)
        *string = 0;
#else
    char *temp = malloc (len);
    char *outp = temp;
    char *inp = string;
    size_t insize = max_chars;
    size_t outsize = len - 1;
    int err = 0;
    char *old_locale;
    iconv_t converter;

    memset(temp, 0, len);
    old_locale = setlocale (LC_CTYPE, "");
    converter = iconv_open ("", "UTF-8");

    if (converter != (iconv_t) -1) {
        err = iconv (converter, &inp, &insize, &outp, &outsize);
        iconv_close (converter);
    }
    else
        err = -1;

    setlocale (LC_CTYPE, old_locale);

    if (err == -1) {
        free(temp);
        return;
    }

    memmove (string, temp, len);
#endif
    free (temp);
}