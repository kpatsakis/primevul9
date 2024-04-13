string_iconv_to_internal (const char *charset, const char *string)
{
    char *input, *output;

    if (!string)
        return NULL;

    input = strdup (string);
    if (!input)
        return NULL;

    /*
     * optimized for UTF-8: if charset is NULL => we use term charset => if
     * this charset is already UTF-8, then no iconv is needed
     */
    if (local_utf8 && (!charset || !charset[0]))
        return input;

    if (utf8_has_8bits (input) && utf8_is_valid (input, NULL))
        return input;

    output = string_iconv (0,
                           (charset && charset[0]) ?
                           charset : weechat_local_charset,
                           WEECHAT_INTERNAL_CHARSET,
                           input);
    if (!output)
        return input;
    utf8_normalize (output, '?');
    free (input);
    return output;
}