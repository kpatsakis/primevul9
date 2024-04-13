yaml_parser_unroll_indent(yaml_parser_t *parser, ptrdiff_t column)
{
    yaml_token_t token;

    /* In the flow context, do nothing. */

    if (parser->flow_level)
        return 1;

    /* Loop through the intendation levels in the stack. */

    while (parser->indent > column)
    {
        /* Create a token and append it to the queue. */

        TOKEN_INIT(token, YAML_BLOCK_END_TOKEN, parser->mark, parser->mark);

        if (!ENQUEUE(parser, parser->tokens, token))
            return 0;

        /* Pop the indentation level. */

        parser->indent = POP(parser, parser->indents);
    }

    return 1;
}