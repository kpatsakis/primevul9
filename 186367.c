yaml_parser_scan_to_next_token(yaml_parser_t *parser)
{
    /* Until the next token is not found. */

    while (1)
    {
        /* Allow the BOM mark to start a line. */

        if (!CACHE(parser, 1)) return 0;

        if (parser->mark.column == 0 && IS_BOM(parser->buffer))
            SKIP(parser);

        /*
         * Eat whitespaces.
         *
         * Tabs are allowed:
         *
         *  - in the flow context;
         *  - in the block context, but not at the beginning of the line or
         *  after '-', '?', or ':' (complex value).  
         */

        if (!CACHE(parser, 1)) return 0;

        while (CHECK(parser->buffer,' ') ||
                ((parser->flow_level || !parser->simple_key_allowed) &&
                 CHECK(parser->buffer, '\t'))) {
            SKIP(parser);
            if (!CACHE(parser, 1)) return 0;
        }

        /* Eat a comment until a line break. */

        if (CHECK(parser->buffer, '#')) {
            while (!IS_BREAKZ(parser->buffer)) {
                SKIP(parser);
                if (!CACHE(parser, 1)) return 0;
            }
        }

        /* If it is a line break, eat it. */

        if (IS_BREAK(parser->buffer))
        {
            if (!CACHE(parser, 2)) return 0;
            SKIP_LINE(parser);

            /* In the block context, a new line may start a simple key. */

            if (!parser->flow_level) {
                parser->simple_key_allowed = 1;
            }
        }
        else
        {
            /* We have found a token. */

            break;
        }
    }

    return 1;
}