yaml_parser_scan_version_directive_number(yaml_parser_t *parser,
        yaml_mark_t start_mark, int *number)
{
    int value = 0;
    size_t length = 0;

    /* Repeat while the next character is digit. */

    if (!CACHE(parser, 1)) return 0;

    while (IS_DIGIT(parser->buffer))
    {
        /* Check if the number is too long. */

        if (++length > MAX_NUMBER_LENGTH) {
            return yaml_parser_set_scanner_error(parser, "while scanning a %YAML directive",
                    start_mark, "found extremely long version number");
        }

        value = value*10 + AS_DIGIT(parser->buffer);

        SKIP(parser);

        if (!CACHE(parser, 1)) return 0;
    }

    /* Check if the number was present. */

    if (!length) {
        return yaml_parser_set_scanner_error(parser, "while scanning a %YAML directive",
                start_mark, "did not find expected version number");
    }

    *number = value;

    return 1;
}