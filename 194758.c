string_iconv_from_internal (const char *charset, const char *string)
{
    char *input, *output;

    if (!string)
        return NULL;

    input = strdup (string);
    if (!input)
        return NULL;

    /*
     * optimized for UTF-8: if charset is NULL => we use term charset => if
     * this charset is already UTF-8, then no iconv needed
     */
    if (local_utf8 && (!charset || !charset[0]))
        return input;

    utf8_normalize (input, '?');
    output = string_iconv (1,
                           WEECHAT_INTERNAL_CHARSET,
                           (charset && charset[0]) ?
                           charset : weechat_local_charset,
                           input);
    if (!output)
        return input;
    free (input);
    return output;
}