yaml_parser_fetch_flow_collection_start(yaml_parser_t *parser,
        yaml_token_type_t type)
{
    yaml_mark_t start_mark, end_mark;
    yaml_token_t token;

    /* The indicators '[' and '{' may start a simple key. */

    if (!yaml_parser_save_simple_key(parser))
        return 0;

    /* Increase the flow level. */

    if (!yaml_parser_increase_flow_level(parser))
        return 0;

    /* A simple key may follow the indicators '[' and '{'. */

    parser->simple_key_allowed = 1;

    /* Consume the token. */

    start_mark = parser->mark;
    SKIP(parser);
    end_mark = parser->mark;

    /* Create the FLOW-SEQUENCE-START of FLOW-MAPPING-START token. */

    TOKEN_INIT(token, type, start_mark, end_mark);

    /* Append the token to the queue. */

    if (!ENQUEUE(parser, parser->tokens, token))
        return 0;

    return 1;
}