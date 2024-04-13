yaml_parser_scan_block_scalar_breaks(yaml_parser_t *parser,
        int *indent, yaml_string_t *breaks,
        yaml_mark_t start_mark, yaml_mark_t *end_mark)
{
    int max_indent = 0;

    *end_mark = parser->mark;

    /* Eat the intendation spaces and line breaks. */

    while (1)
    {
        /* Eat the intendation spaces. */

        if (!CACHE(parser, 1)) return 0;

        while ((!*indent || (int)parser->mark.column < *indent)
                && IS_SPACE(parser->buffer)) {
            SKIP(parser);
            if (!CACHE(parser, 1)) return 0;
        }

        if ((int)parser->mark.column > max_indent)
            max_indent = (int)parser->mark.column;

        /* Check for a tab character messing the intendation. */

        if ((!*indent || (int)parser->mark.column < *indent)
                && IS_TAB(parser->buffer)) {
            return yaml_parser_set_scanner_error(parser, "while scanning a block scalar",
                    start_mark, "found a tab character where an intendation space is expected");
        }

        /* Have we found a non-empty line? */

        if (!IS_BREAK(parser->buffer)) break;

        /* Consume the line break. */

        if (!CACHE(parser, 2)) return 0;
        if (!READ_LINE(parser, *breaks)) return 0;
        *end_mark = parser->mark;
    }

    /* Determine the indentation level if needed. */

    if (!*indent) {
        *indent = max_indent;
        if (*indent < parser->indent + 1)
            *indent = parser->indent + 1;
        if (*indent < 1)
            *indent = 1;
    }

   return 1; 
}