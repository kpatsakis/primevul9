yaml_parser_fetch_block_entry(yaml_parser_t *parser)
{
    yaml_mark_t start_mark, end_mark;
    yaml_token_t token;

    /* Check if the scanner is in the block context. */

    if (!parser->flow_level)
    {
        /* Check if we are allowed to start a new entry. */

        if (!parser->simple_key_allowed) {
            return yaml_parser_set_scanner_error(parser, NULL, parser->mark,
                    "block sequence entries are not allowed in this context");
        }

        /* Add the BLOCK-SEQUENCE-START token if needed. */

        if (!yaml_parser_roll_indent(parser, parser->mark.column, -1,
                    YAML_BLOCK_SEQUENCE_START_TOKEN, parser->mark))
            return 0;
    }
    else
    {
        /*
         * It is an error for the '-' indicator to occur in the flow context,
         * but we let the Parser detect and report about it because the Parser
         * is able to point to the context.
         */
    }

    /* Reset any potential simple keys on the current flow level. */

    if (!yaml_parser_remove_simple_key(parser))
        return 0;

    /* Simple keys are allowed after '-'. */

    parser->simple_key_allowed = 1;

    /* Consume the token. */

    start_mark = parser->mark;
    SKIP(parser);
    end_mark = parser->mark;

    /* Create the BLOCK-ENTRY token and append it to the queue. */

    TOKEN_INIT(token, YAML_BLOCK_ENTRY_TOKEN, start_mark, end_mark);

    if (!ENQUEUE(parser, parser->tokens, token))
        return 0;

    return 1;
}