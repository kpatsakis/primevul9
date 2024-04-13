yaml_parser_fetch_tag(yaml_parser_t *parser)
{
    yaml_token_t token;

    /* A tag could be a simple key. */

    if (!yaml_parser_save_simple_key(parser))
        return 0;

    /* A simple key cannot follow a tag. */

    parser->simple_key_allowed = 0;

    /* Create the TAG token and append it to the queue. */

    if (!yaml_parser_scan_tag(parser, &token))
        return 0;

    if (!ENQUEUE(parser, parser->tokens, token)) {
        yaml_token_delete(&token);
        return 0;
    }

    return 1;
}