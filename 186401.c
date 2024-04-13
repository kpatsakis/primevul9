yaml_parser_scan_flow_scalar(yaml_parser_t *parser, yaml_token_t *token,
        int single)
{
    yaml_mark_t start_mark;
    yaml_mark_t end_mark;
    yaml_string_t string = NULL_STRING;
    yaml_string_t leading_break = NULL_STRING;
    yaml_string_t trailing_breaks = NULL_STRING;
    yaml_string_t whitespaces = NULL_STRING;
    int leading_blanks;

    if (!STRING_INIT(parser, string, INITIAL_STRING_SIZE)) goto error;
    if (!STRING_INIT(parser, leading_break, INITIAL_STRING_SIZE)) goto error;
    if (!STRING_INIT(parser, trailing_breaks, INITIAL_STRING_SIZE)) goto error;
    if (!STRING_INIT(parser, whitespaces, INITIAL_STRING_SIZE)) goto error;

    /* Eat the left quote. */

    start_mark = parser->mark;

    SKIP(parser);

    /* Consume the content of the quoted scalar. */

    while (1)
    {
        /* Check that there are no document indicators at the beginning of the line. */

        if (!CACHE(parser, 4)) goto error;

        if (parser->mark.column == 0 &&
            ((CHECK_AT(parser->buffer, '-', 0) &&
              CHECK_AT(parser->buffer, '-', 1) &&
              CHECK_AT(parser->buffer, '-', 2)) ||
             (CHECK_AT(parser->buffer, '.', 0) &&
              CHECK_AT(parser->buffer, '.', 1) &&
              CHECK_AT(parser->buffer, '.', 2))) &&
            IS_BLANKZ_AT(parser->buffer, 3))
        {
            yaml_parser_set_scanner_error(parser, "while scanning a quoted scalar",
                    start_mark, "found unexpected document indicator");
            goto error;
        }

        /* Check for EOF. */

        if (IS_Z(parser->buffer)) {
            yaml_parser_set_scanner_error(parser, "while scanning a quoted scalar",
                    start_mark, "found unexpected end of stream");
            goto error;
        }

        /* Consume non-blank characters. */

        if (!CACHE(parser, 2)) goto error;

        leading_blanks = 0;

        while (!IS_BLANKZ(parser->buffer))
        {
            /* Check for an escaped single quote. */

            if (single && CHECK_AT(parser->buffer, '\'', 0)
                    && CHECK_AT(parser->buffer, '\'', 1))
            {
                if (!STRING_EXTEND(parser, string)) goto error;
                *(string.pointer++) = '\'';
                SKIP(parser);
                SKIP(parser);
            }

            /* Check for the right quote. */

            else if (CHECK(parser->buffer, single ? '\'' : '"'))
            {
                break;
            }

            /* Check for an escaped line break. */

            else if (!single && CHECK(parser->buffer, '\\')
                    && IS_BREAK_AT(parser->buffer, 1))
            {
                if (!CACHE(parser, 3)) goto error;
                SKIP(parser);
                SKIP_LINE(parser);
                leading_blanks = 1;
                break;
            }

            /* Check for an escape sequence. */

            else if (!single && CHECK(parser->buffer, '\\'))
            {
                size_t code_length = 0;

                if (!STRING_EXTEND(parser, string)) goto error;

                /* Check the escape character. */

                switch (parser->buffer.pointer[1])
                {
                    case '0':
                        *(string.pointer++) = '\0';
                        break;

                    case 'a':
                        *(string.pointer++) = '\x07';
                        break;

                    case 'b':
                        *(string.pointer++) = '\x08';
                        break;

                    case 't':
                    case '\t':
                        *(string.pointer++) = '\x09';
                        break;

                    case 'n':
                        *(string.pointer++) = '\x0A';
                        break;

                    case 'v':
                        *(string.pointer++) = '\x0B';
                        break;

                    case 'f':
                        *(string.pointer++) = '\x0C';
                        break;

                    case 'r':
                        *(string.pointer++) = '\x0D';
                        break;

                    case 'e':
                        *(string.pointer++) = '\x1B';
                        break;

                    case ' ':
                        *(string.pointer++) = '\x20';
                        break;

                    case '"':
                        *(string.pointer++) = '"';
                        break;

                    case '\'':
                        *(string.pointer++) = '\'';
                        break;

                    case '\\':
                        *(string.pointer++) = '\\';
                        break;

                    case 'N':   /* NEL (#x85) */
                        *(string.pointer++) = '\xC2';
                        *(string.pointer++) = '\x85';
                        break;

                    case '_':   /* #xA0 */
                        *(string.pointer++) = '\xC2';
                        *(string.pointer++) = '\xA0';
                        break;

                    case 'L':   /* LS (#x2028) */
                        *(string.pointer++) = '\xE2';
                        *(string.pointer++) = '\x80';
                        *(string.pointer++) = '\xA8';
                        break;

                    case 'P':   /* PS (#x2029) */
                        *(string.pointer++) = '\xE2';
                        *(string.pointer++) = '\x80';
                        *(string.pointer++) = '\xA9';
                        break;

                    case 'x':
                        code_length = 2;
                        break;

                    case 'u':
                        code_length = 4;
                        break;

                    case 'U':
                        code_length = 8;
                        break;

                    default:
                        yaml_parser_set_scanner_error(parser, "while parsing a quoted scalar",
                                start_mark, "found unknown escape character");
                        goto error;
                }

                SKIP(parser);
                SKIP(parser);

                /* Consume an arbitrary escape code. */

                if (code_length)
                {
                    unsigned int value = 0;
                    size_t k;

                    /* Scan the character value. */

                    if (!CACHE(parser, code_length)) goto error;

                    for (k = 0; k < code_length; k ++) {
                        if (!IS_HEX_AT(parser->buffer, k)) {
                            yaml_parser_set_scanner_error(parser, "while parsing a quoted scalar",
                                    start_mark, "did not find expected hexdecimal number");
                            goto error;
                        }
                        value = (value << 4) + AS_HEX_AT(parser->buffer, k);
                    }

                    /* Check the value and write the character. */

                    if ((value >= 0xD800 && value <= 0xDFFF) || value > 0x10FFFF) {
                        yaml_parser_set_scanner_error(parser, "while parsing a quoted scalar",
                                start_mark, "found invalid Unicode character escape code");
                        goto error;
                    }

                    if (value <= 0x7F) {
                        *(string.pointer++) = value;
                    }
                    else if (value <= 0x7FF) {
                        *(string.pointer++) = 0xC0 + (value >> 6);
                        *(string.pointer++) = 0x80 + (value & 0x3F);
                    }
                    else if (value <= 0xFFFF) {
                        *(string.pointer++) = 0xE0 + (value >> 12);
                        *(string.pointer++) = 0x80 + ((value >> 6) & 0x3F);
                        *(string.pointer++) = 0x80 + (value & 0x3F);
                    }
                    else {
                        *(string.pointer++) = 0xF0 + (value >> 18);
                        *(string.pointer++) = 0x80 + ((value >> 12) & 0x3F);
                        *(string.pointer++) = 0x80 + ((value >> 6) & 0x3F);
                        *(string.pointer++) = 0x80 + (value & 0x3F);
                    }

                    /* Advance the pointer. */

                    for (k = 0; k < code_length; k ++) {
                        SKIP(parser);
                    }
                }
            }

            else
            {
                /* It is a non-escaped non-blank character. */

                if (!READ(parser, string)) goto error;
            }

            if (!CACHE(parser, 2)) goto error;
        }

        /* Check if we are at the end of the scalar. */

        if (CHECK(parser->buffer, single ? '\'' : '"'))
            break;

        /* Consume blank characters. */

        if (!CACHE(parser, 1)) goto error;

        while (IS_BLANK(parser->buffer) || IS_BREAK(parser->buffer))
        {
            if (IS_BLANK(parser->buffer))
            {
                /* Consume a space or a tab character. */

                if (!leading_blanks) {
                    if (!READ(parser, whitespaces)) goto error;
                }
                else {
                    SKIP(parser);
                }
            }
            else
            {
                if (!CACHE(parser, 2)) goto error;

                /* Check if it is a first line break. */

                if (!leading_blanks)
                {
                    CLEAR(parser, whitespaces);
                    if (!READ_LINE(parser, leading_break)) goto error;
                    leading_blanks = 1;
                }
                else
                {
                    if (!READ_LINE(parser, trailing_breaks)) goto error;
                }
            }
            if (!CACHE(parser, 1)) goto error;
        }

        /* Join the whitespaces or fold line breaks. */

        if (leading_blanks)
        {
            /* Do we need to fold line breaks? */

            if (leading_break.start[0] == '\n') {
                if (trailing_breaks.start[0] == '\0') {
                    if (!STRING_EXTEND(parser, string)) goto error;
                    *(string.pointer++) = ' ';
                }
                else {
                    if (!JOIN(parser, string, trailing_breaks)) goto error;
                    CLEAR(parser, trailing_breaks);
                }
                CLEAR(parser, leading_break);
            }
            else {
                if (!JOIN(parser, string, leading_break)) goto error;
                if (!JOIN(parser, string, trailing_breaks)) goto error;
                CLEAR(parser, leading_break);
                CLEAR(parser, trailing_breaks);
            }
        }
        else
        {
            if (!JOIN(parser, string, whitespaces)) goto error;
            CLEAR(parser, whitespaces);
        }
    }

    /* Eat the right quote. */

    SKIP(parser);

    end_mark = parser->mark;

    /* Create a token. */

    SCALAR_TOKEN_INIT(*token, string.start, string.pointer-string.start,
            single ? YAML_SINGLE_QUOTED_SCALAR_STYLE : YAML_DOUBLE_QUOTED_SCALAR_STYLE,
            start_mark, end_mark);

    STRING_DEL(parser, leading_break);
    STRING_DEL(parser, trailing_breaks);
    STRING_DEL(parser, whitespaces);

    return 1;

error:
    STRING_DEL(parser, string);
    STRING_DEL(parser, leading_break);
    STRING_DEL(parser, trailing_breaks);
    STRING_DEL(parser, whitespaces);

    return 0;
}