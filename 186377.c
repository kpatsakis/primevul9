yaml_parser_fetch_next_token(yaml_parser_t *parser)
{
    /* Ensure that the buffer is initialized. */

    if (!CACHE(parser, 1))
        return 0;

    /* Check if we just started scanning.  Fetch STREAM-START then. */

    if (!parser->stream_start_produced)
        return yaml_parser_fetch_stream_start(parser);

    /* Eat whitespaces and comments until we reach the next token. */

    if (!yaml_parser_scan_to_next_token(parser))
        return 0;

    /* Remove obsolete potential simple keys. */

    if (!yaml_parser_stale_simple_keys(parser))
        return 0;

    /* Check the indentation level against the current column. */

    if (!yaml_parser_unroll_indent(parser, parser->mark.column))
        return 0;

    /*
     * Ensure that the buffer contains at least 4 characters.  4 is the length
     * of the longest indicators ('--- ' and '... ').
     */

    if (!CACHE(parser, 4))
        return 0;

    /* Is it the end of the stream? */

    if (IS_Z(parser->buffer))
        return yaml_parser_fetch_stream_end(parser);

    /* Is it a directive? */

    if (parser->mark.column == 0 && CHECK(parser->buffer, '%'))
        return yaml_parser_fetch_directive(parser);

    /* Is it the document start indicator? */

    if (parser->mark.column == 0
            && CHECK_AT(parser->buffer, '-', 0)
            && CHECK_AT(parser->buffer, '-', 1)
            && CHECK_AT(parser->buffer, '-', 2)
            && IS_BLANKZ_AT(parser->buffer, 3))
        return yaml_parser_fetch_document_indicator(parser,
                YAML_DOCUMENT_START_TOKEN);

    /* Is it the document end indicator? */

    if (parser->mark.column == 0
            && CHECK_AT(parser->buffer, '.', 0)
            && CHECK_AT(parser->buffer, '.', 1)
            && CHECK_AT(parser->buffer, '.', 2)
            && IS_BLANKZ_AT(parser->buffer, 3))
        return yaml_parser_fetch_document_indicator(parser,
                YAML_DOCUMENT_END_TOKEN);

    /* Is it the flow sequence start indicator? */

    if (CHECK(parser->buffer, '['))
        return yaml_parser_fetch_flow_collection_start(parser,
                YAML_FLOW_SEQUENCE_START_TOKEN);

    /* Is it the flow mapping start indicator? */

    if (CHECK(parser->buffer, '{'))
        return yaml_parser_fetch_flow_collection_start(parser,
                YAML_FLOW_MAPPING_START_TOKEN);

    /* Is it the flow sequence end indicator? */

    if (CHECK(parser->buffer, ']'))
        return yaml_parser_fetch_flow_collection_end(parser,
                YAML_FLOW_SEQUENCE_END_TOKEN);

    /* Is it the flow mapping end indicator? */

    if (CHECK(parser->buffer, '}'))
        return yaml_parser_fetch_flow_collection_end(parser,
                YAML_FLOW_MAPPING_END_TOKEN);

    /* Is it the flow entry indicator? */

    if (CHECK(parser->buffer, ','))
        return yaml_parser_fetch_flow_entry(parser);

    /* Is it the block entry indicator? */

    if (CHECK(parser->buffer, '-') && IS_BLANKZ_AT(parser->buffer, 1))
        return yaml_parser_fetch_block_entry(parser);

    /* Is it the key indicator? */

    if (CHECK(parser->buffer, '?')
            && (parser->flow_level || IS_BLANKZ_AT(parser->buffer, 1)))
        return yaml_parser_fetch_key(parser);

    /* Is it the value indicator? */

    if (CHECK(parser->buffer, ':')
            && (parser->flow_level || IS_BLANKZ_AT(parser->buffer, 1)))
        return yaml_parser_fetch_value(parser);

    /* Is it an alias? */

    if (CHECK(parser->buffer, '*'))
        return yaml_parser_fetch_anchor(parser, YAML_ALIAS_TOKEN);

    /* Is it an anchor? */

    if (CHECK(parser->buffer, '&'))
        return yaml_parser_fetch_anchor(parser, YAML_ANCHOR_TOKEN);

    /* Is it a tag? */

    if (CHECK(parser->buffer, '!'))
        return yaml_parser_fetch_tag(parser);

    /* Is it a literal scalar? */

    if (CHECK(parser->buffer, '|') && !parser->flow_level)
        return yaml_parser_fetch_block_scalar(parser, 1);

    /* Is it a folded scalar? */

    if (CHECK(parser->buffer, '>') && !parser->flow_level)
        return yaml_parser_fetch_block_scalar(parser, 0);

    /* Is it a single-quoted scalar? */

    if (CHECK(parser->buffer, '\''))
        return yaml_parser_fetch_flow_scalar(parser, 1);

    /* Is it a double-quoted scalar? */

    if (CHECK(parser->buffer, '"'))
        return yaml_parser_fetch_flow_scalar(parser, 0);

    /*
     * Is it a plain scalar?
     *
     * A plain scalar may start with any non-blank characters except
     *
     *      '-', '?', ':', ',', '[', ']', '{', '}',
     *      '#', '&', '*', '!', '|', '>', '\'', '\"',
     *      '%', '@', '`'.
     *
     * In the block context (and, for the '-' indicator, in the flow context
     * too), it may also start with the characters
     *
     *      '-', '?', ':'
     *
     * if it is followed by a non-space character.
     *
     * The last rule is more restrictive than the specification requires.
     */

    if (!(IS_BLANKZ(parser->buffer) || CHECK(parser->buffer, '-')
                || CHECK(parser->buffer, '?') || CHECK(parser->buffer, ':')
                || CHECK(parser->buffer, ',') || CHECK(parser->buffer, '[')
                || CHECK(parser->buffer, ']') || CHECK(parser->buffer, '{')
                || CHECK(parser->buffer, '}') || CHECK(parser->buffer, '#')
                || CHECK(parser->buffer, '&') || CHECK(parser->buffer, '*')
                || CHECK(parser->buffer, '!') || CHECK(parser->buffer, '|')
                || CHECK(parser->buffer, '>') || CHECK(parser->buffer, '\'')
                || CHECK(parser->buffer, '"') || CHECK(parser->buffer, '%')
                || CHECK(parser->buffer, '@') || CHECK(parser->buffer, '`')) ||
            (CHECK(parser->buffer, '-') && !IS_BLANK_AT(parser->buffer, 1)) ||
            (!parser->flow_level &&
             (CHECK(parser->buffer, '?') || CHECK(parser->buffer, ':'))
             && !IS_BLANKZ_AT(parser->buffer, 1)))
        return yaml_parser_fetch_plain_scalar(parser);

    /*
     * If we don't determine the token type so far, it is an error.
     */

    return yaml_parser_set_scanner_error(parser,
            "while scanning for the next token", parser->mark,
            "found character that cannot start any token");
}