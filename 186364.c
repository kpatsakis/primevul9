yaml_parser_fetch_flow_scalar(yaml_parser_t *parser, int single)
{
    yaml_token_t token;

    /* A plain scalar could be a simple key. */

    if (!yaml_parser_save_simple_key(parser))
        return 0;

    /* A simple key cannot follow a flow scalar. */

    parser->simple_key_allowed = 0;

    /* Create the SCALAR token and append it to the queue. */

    if (!yaml_parser_scan_flow_scalar(parser, &token, single))
        return 0;

    if (!ENQUEUE(parser, parser->tokens, token)) {
        yaml_token_delete(&token);
        return 0;
    }

    return 1;
}