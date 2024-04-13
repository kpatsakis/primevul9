yaml_parser_fetch_value(yaml_parser_t *parser)
{
    yaml_mark_t start_mark, end_mark;
    yaml_token_t token;
    yaml_simple_key_t *simple_key = parser->simple_keys.top-1;

    /* Have we found a simple key? */

    if (simple_key->possible)
    {

        /* Create the KEY token and insert it into the queue. */

        TOKEN_INIT(token, YAML_KEY_TOKEN, simple_key->mark, simple_key->mark);

        if (!QUEUE_INSERT(parser, parser->tokens,
                    simple_key->token_number - parser->tokens_parsed, token))
            return 0;

        /* In the block context, we may need to add the BLOCK-MAPPING-START token. */

        if (!yaml_parser_roll_indent(parser, simple_key->mark.column,
                    simple_key->token_number,
                    YAML_BLOCK_MAPPING_START_TOKEN, simple_key->mark))
            return 0;

        /* Remove the simple key. */

        simple_key->possible = 0;

        /* A simple key cannot follow another simple key. */

        parser->simple_key_allowed = 0;
    }
    else
    {
        /* The ':' indicator follows a complex key. */

        /* In the block context, extra checks are required. */

        if (!parser->flow_level)
        {
            /* Check if we are allowed to start a complex value. */

            if (!parser->simple_key_allowed) {
                return yaml_parser_set_scanner_error(parser, NULL, parser->mark,
                        "mapping values are not allowed in this context");
            }

            /* Add the BLOCK-MAPPING-START token if needed. */

            if (!yaml_parser_roll_indent(parser, parser->mark.column, -1,
                        YAML_BLOCK_MAPPING_START_TOKEN, parser->mark))
                return 0;
        }

        /* Simple keys after ':' are allowed in the block context. */

        parser->simple_key_allowed = (!parser->flow_level);
    }

    /* Consume the token. */

    start_mark = parser->mark;
    SKIP(parser);
    end_mark = parser->mark;

    /* Create the VALUE token and append it to the queue. */

    TOKEN_INIT(token, YAML_VALUE_TOKEN, start_mark, end_mark);

    if (!ENQUEUE(parser, parser->tokens, token))
        return 0;

    return 1;
}