string_is_word_char (const char *string)
{
    wint_t c = utf8_wide_char (string);

    if (c == WEOF)
        return 0;

    if (iswalnum (c))
        return 1;

    switch (c)
    {
        case '-':
        case '_':
        case '|':
            return 1;
    }

    /* not a 'word char' */
    return 0;
}