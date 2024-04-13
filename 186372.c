yaml_parser_scan_anchor(yaml_parser_t *parser, yaml_token_t *token,
        yaml_token_type_t type)
{
    int length = 0;
    yaml_mark_t start_mark, end_mark;
    yaml_string_t string = NULL_STRING;

    if (!STRING_INIT(parser, string, INITIAL_STRING_SIZE)) goto error;

    /* Eat the indicator character. */

    start_mark = parser->mark;

    SKIP(parser);

    /* Consume the value. */

    if (!CACHE(parser, 1)) goto error;

    while (IS_ALPHA(parser->buffer)) {
        if (!READ(parser, string)) goto error;
        if (!CACHE(parser, 1)) goto error;
        length ++;
    }

    end_mark = parser->mark;

    /*
     * Check if length of the anchor is greater than 0 and it is followed by
     * a whitespace character or one of the indicators:
     *
     *      '?', ':', ',', ']', '}', '%', '@', '`'.
     */

    if (!length || !(IS_BLANKZ(parser->buffer) || CHECK(parser->buffer, '?')
                || CHECK(parser->buffer, ':') || CHECK(parser->buffer, ',')
                || CHECK(parser->buffer, ']') || CHECK(parser->buffer, '}')
                || CHECK(parser->buffer, '%') || CHECK(parser->buffer, '@')
                || CHECK(parser->buffer, '`'))) {
        yaml_parser_set_scanner_error(parser, type == YAML_ANCHOR_TOKEN ?
                "while scanning an anchor" : "while scanning an alias", start_mark,
                "did not find expected alphabetic or numeric character");
        goto error;
    }

    /* Create a token. */

    if (type == YAML_ANCHOR_TOKEN) {
        ANCHOR_TOKEN_INIT(*token, string.start, start_mark, end_mark);
    }
    else {
        ALIAS_TOKEN_INIT(*token, string.start, start_mark, end_mark);
    }

    return 1;

error:
    STRING_DEL(parser, string);
    return 0;
}