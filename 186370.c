yaml_parser_roll_indent(yaml_parser_t *parser, ptrdiff_t column,
        ptrdiff_t number, yaml_token_type_t type, yaml_mark_t mark)
{
    yaml_token_t token;

    /* In the flow context, do nothing. */

    if (parser->flow_level)
        return 1;

    if (parser->indent < column)
    {
        /*
         * Push the current indentation level to the stack and set the new
         * indentation level.
         */

        if (!PUSH(parser, parser->indents, parser->indent))
            return 0;

        if (column > INT_MAX) {
            parser->error = YAML_MEMORY_ERROR;
            return 0;
        }

        parser->indent = column;

        /* Create a token and insert it into the queue. */

        TOKEN_INIT(token, type, mark, mark);

        if (number == -1) {
            if (!ENQUEUE(parser, parser->tokens, token))
                return 0;
        }
        else {
            if (!QUEUE_INSERT(parser,
                        parser->tokens, number - parser->tokens_parsed, token))
                return 0;
        }
    }

    return 1;
}