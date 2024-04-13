yaml_parser_scan_block_scalar(yaml_parser_t *parser, yaml_token_t *token,
        int literal)
{
    yaml_mark_t start_mark;
    yaml_mark_t end_mark;
    yaml_string_t string = NULL_STRING;
    yaml_string_t leading_break = NULL_STRING;
    yaml_string_t trailing_breaks = NULL_STRING;
    int chomping = 0;
    int increment = 0;
    int indent = 0;
    int leading_blank = 0;
    int trailing_blank = 0;

    if (!STRING_INIT(parser, string, INITIAL_STRING_SIZE)) goto error;
    if (!STRING_INIT(parser, leading_break, INITIAL_STRING_SIZE)) goto error;
    if (!STRING_INIT(parser, trailing_breaks, INITIAL_STRING_SIZE)) goto error;

    /* Eat the indicator '|' or '>'. */

    start_mark = parser->mark;

    SKIP(parser);

    /* Scan the additional block scalar indicators. */

    if (!CACHE(parser, 1)) goto error;

    /* Check for a chomping indicator. */

    if (CHECK(parser->buffer, '+') || CHECK(parser->buffer, '-'))
    {
        /* Set the chomping method and eat the indicator. */

        chomping = CHECK(parser->buffer, '+') ? +1 : -1;

        SKIP(parser);

        /* Check for an indentation indicator. */

        if (!CACHE(parser, 1)) goto error;

        if (IS_DIGIT(parser->buffer))
        {
            /* Check that the intendation is greater than 0. */

            if (CHECK(parser->buffer, '0')) {
                yaml_parser_set_scanner_error(parser, "while scanning a block scalar",
                        start_mark, "found an intendation indicator equal to 0");
                goto error;
            }

            /* Get the intendation level and eat the indicator. */

            increment = AS_DIGIT(parser->buffer);

            SKIP(parser);
        }
    }

    /* Do the same as above, but in the opposite order. */

    else if (IS_DIGIT(parser->buffer))
    {
        if (CHECK(parser->buffer, '0')) {
            yaml_parser_set_scanner_error(parser, "while scanning a block scalar",
                    start_mark, "found an intendation indicator equal to 0");
            goto error;
        }

        increment = AS_DIGIT(parser->buffer);

        SKIP(parser);

        if (!CACHE(parser, 1)) goto error;

        if (CHECK(parser->buffer, '+') || CHECK(parser->buffer, '-')) {
            chomping = CHECK(parser->buffer, '+') ? +1 : -1;

            SKIP(parser);
        }
    }

    /* Eat whitespaces and comments to the end of the line. */

    if (!CACHE(parser, 1)) goto error;

    while (IS_BLANK(parser->buffer)) {
        SKIP(parser);
        if (!CACHE(parser, 1)) goto error;
    }

    if (CHECK(parser->buffer, '#')) {
        while (!IS_BREAKZ(parser->buffer)) {
            SKIP(parser);
            if (!CACHE(parser, 1)) goto error;
        }
    }

    /* Check if we are at the end of the line. */

    if (!IS_BREAKZ(parser->buffer)) {
        yaml_parser_set_scanner_error(parser, "while scanning a block scalar",
                start_mark, "did not find expected comment or line break");
        goto error;
    }

    /* Eat a line break. */

    if (IS_BREAK(parser->buffer)) {
        if (!CACHE(parser, 2)) goto error;
        SKIP_LINE(parser);
    }

    end_mark = parser->mark;

    /* Set the intendation level if it was specified. */

    if (increment) {
        indent = parser->indent >= 0 ? parser->indent+increment : increment;
    }

    /* Scan the leading line breaks and determine the indentation level if needed. */

    if (!yaml_parser_scan_block_scalar_breaks(parser, &indent, &trailing_breaks,
                start_mark, &end_mark)) goto error;

    /* Scan the block scalar content. */

    if (!CACHE(parser, 1)) goto error;

    while ((int)parser->mark.column == indent && !IS_Z(parser->buffer))
    {
        /*
         * We are at the beginning of a non-empty line.
         */

        /* Is it a trailing whitespace? */

        trailing_blank = IS_BLANK(parser->buffer);

        /* Check if we need to fold the leading line break. */

        if (!literal && (*leading_break.start == '\n')
                && !leading_blank && !trailing_blank)
        {
            /* Do we need to join the lines by space? */

            if (*trailing_breaks.start == '\0') {
                if (!STRING_EXTEND(parser, string)) goto error;
                *(string.pointer ++) = ' ';
            }

            CLEAR(parser, leading_break);
        }
        else {
            if (!JOIN(parser, string, leading_break)) goto error;
            CLEAR(parser, leading_break);
        }

        /* Append the remaining line breaks. */

        if (!JOIN(parser, string, trailing_breaks)) goto error;
        CLEAR(parser, trailing_breaks);

        /* Is it a leading whitespace? */

        leading_blank = IS_BLANK(parser->buffer);

        /* Consume the current line. */

        while (!IS_BREAKZ(parser->buffer)) {
            if (!READ(parser, string)) goto error;
            if (!CACHE(parser, 1)) goto error;
        }

        /* Consume the line break. */

        if (!CACHE(parser, 2)) goto error;

        if (!READ_LINE(parser, leading_break)) goto error;

        /* Eat the following intendation spaces and line breaks. */

        if (!yaml_parser_scan_block_scalar_breaks(parser,
                    &indent, &trailing_breaks, start_mark, &end_mark)) goto error;
    }

    /* Chomp the tail. */

    if (chomping != -1) {
        if (!JOIN(parser, string, leading_break)) goto error;
    }
    if (chomping == 1) {
        if (!JOIN(parser, string, trailing_breaks)) goto error;
    }

    /* Create a token. */

    SCALAR_TOKEN_INIT(*token, string.start, string.pointer-string.start,
            literal ? YAML_LITERAL_SCALAR_STYLE : YAML_FOLDED_SCALAR_STYLE,
            start_mark, end_mark);

    STRING_DEL(parser, leading_break);
    STRING_DEL(parser, trailing_breaks);

    return 1;

error:
    STRING_DEL(parser, string);
    STRING_DEL(parser, leading_break);
    STRING_DEL(parser, trailing_breaks);

    return 0;
}