yaml_parser_scan_tag_handle(yaml_parser_t *parser, int directive,
        yaml_mark_t start_mark, yaml_char_t **handle)
{
    yaml_string_t string = NULL_STRING;

    if (!STRING_INIT(parser, string, INITIAL_STRING_SIZE)) goto error;

    /* Check the initial '!' character. */

    if (!CACHE(parser, 1)) goto error;

    if (!CHECK(parser->buffer, '!')) {
        yaml_parser_set_scanner_error(parser, directive ?
                "while scanning a tag directive" : "while scanning a tag",
                start_mark, "did not find expected '!'");
        goto error;
    }

    /* Copy the '!' character. */

    if (!READ(parser, string)) goto error;

    /* Copy all subsequent alphabetical and numerical characters. */

    if (!CACHE(parser, 1)) goto error;

    while (IS_ALPHA(parser->buffer))
    {
        if (!READ(parser, string)) goto error;
        if (!CACHE(parser, 1)) goto error;
    }

    /* Check if the trailing character is '!' and copy it. */

    if (CHECK(parser->buffer, '!'))
    {
        if (!READ(parser, string)) goto error;
    }
    else
    {
        /*
         * It's either the '!' tag or not really a tag handle.  If it's a %TAG
         * directive, it's an error.  If it's a tag token, it must be a part of
         * URI.
         */

        if (directive && !(string.start[0] == '!' && string.start[1] == '\0')) {
            yaml_parser_set_scanner_error(parser, "while parsing a tag directive",
                    start_mark, "did not find expected '!'");
            goto error;
        }
    }

    *handle = string.start;

    return 1;

error:
    STRING_DEL(parser, string);
    return 0;
}