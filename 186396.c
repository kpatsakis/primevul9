yaml_parser_fetch_stream_end(yaml_parser_t *parser)
{
    yaml_token_t token;

    /* Force new line. */

    if (parser->mark.column != 0) {
        parser->mark.column = 0;
        parser->mark.line ++;
    }

    /* Reset the indentation level. */

    if (!yaml_parser_unroll_indent(parser, -1))
        return 0;

    /* Reset simple keys. */

    if (!yaml_parser_remove_simple_key(parser))
        return 0;

    parser->simple_key_allowed = 0;

    /* Create the STREAM-END token and append it to the queue. */

    STREAM_END_TOKEN_INIT(token, parser->mark, parser->mark);

    if (!ENQUEUE(parser, parser->tokens, token))
        return 0;

    return 1;
}