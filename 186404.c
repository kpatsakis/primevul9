yaml_parser_scan_tag_uri(yaml_parser_t *parser, int directive,
        yaml_char_t *head, yaml_mark_t start_mark, yaml_char_t **uri)
{
    size_t length = head ? strlen((char *)head) : 0;
    yaml_string_t string = NULL_STRING;

    if (!STRING_INIT(parser, string, INITIAL_STRING_SIZE)) goto error;

    /* Resize the string to include the head. */

    while ((size_t)(string.end - string.start) <= length) {
        if (!yaml_string_extend(&string.start, &string.pointer, &string.end)) {
            parser->error = YAML_MEMORY_ERROR;
            goto error;
        }
    }

    /*
     * Copy the head if needed.
     *
     * Note that we don't copy the leading '!' character.
     */

    if (length > 1) {
        memcpy(string.start, head+1, length-1);
        string.pointer += length-1;
    }

    /* Scan the tag. */

    if (!CACHE(parser, 1)) goto error;

    /*
     * The set of characters that may appear in URI is as follows:
     *
     *      '0'-'9', 'A'-'Z', 'a'-'z', '_', '-', ';', '/', '?', ':', '@', '&',
     *      '=', '+', '$', ',', '.', '!', '~', '*', '\'', '(', ')', '[', ']',
     *      '%'.
     */

    while (IS_ALPHA(parser->buffer) || CHECK(parser->buffer, ';')
            || CHECK(parser->buffer, '/') || CHECK(parser->buffer, '?')
            || CHECK(parser->buffer, ':') || CHECK(parser->buffer, '@')
            || CHECK(parser->buffer, '&') || CHECK(parser->buffer, '=')
            || CHECK(parser->buffer, '+') || CHECK(parser->buffer, '$')
            || CHECK(parser->buffer, ',') || CHECK(parser->buffer, '.')
            || CHECK(parser->buffer, '!') || CHECK(parser->buffer, '~')
            || CHECK(parser->buffer, '*') || CHECK(parser->buffer, '\'')
            || CHECK(parser->buffer, '(') || CHECK(parser->buffer, ')')
            || CHECK(parser->buffer, '[') || CHECK(parser->buffer, ']')
            || CHECK(parser->buffer, '%'))
    {
        /* Check if it is a URI-escape sequence. */

        if (CHECK(parser->buffer, '%')) {
            if (!STRING_EXTEND(parser, string))
                goto error;

            if (!yaml_parser_scan_uri_escapes(parser,
                        directive, start_mark, &string)) goto error;
        }
        else {
            if (!READ(parser, string)) goto error;
        }

        length ++;
        if (!CACHE(parser, 1)) goto error;
    }

    /* Check if the tag is non-empty. */

    if (!length) {
        if (!STRING_EXTEND(parser, string))
            goto error;

        yaml_parser_set_scanner_error(parser, directive ?
                "while parsing a %TAG directive" : "while parsing a tag",
                start_mark, "did not find expected tag URI");
        goto error;
    }

    *uri = string.start;

    return 1;

error:
    STRING_DEL(parser, string);
    return 0;
}