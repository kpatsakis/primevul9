yaml_parser_fetch_directive(yaml_parser_t *parser)
{
    yaml_token_t token;

    /* Reset the indentation level. */

    if (!yaml_parser_unroll_indent(parser, -1))
        return 0;

    /* Reset simple keys. */

    if (!yaml_parser_remove_simple_key(parser))
        return 0;

    parser->simple_key_allowed = 0;

    /* Create the YAML-DIRECTIVE or TAG-DIRECTIVE token. */

    if (!yaml_parser_scan_directive(parser, &token))
        return 0;

    /* Append the token to the queue. */

    if (!ENQUEUE(parser, parser->tokens, token)) {
        yaml_token_delete(&token);
        return 0;
    }

    return 1;
}