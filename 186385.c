yaml_parser_scan_directive_name(yaml_parser_t *parser,
        yaml_mark_t start_mark, yaml_char_t **name)
{
    yaml_string_t string = NULL_STRING;

    if (!STRING_INIT(parser, string, INITIAL_STRING_SIZE)) goto error;

    /* Consume the directive name. */

    if (!CACHE(parser, 1)) goto error;

    while (IS_ALPHA(parser->buffer))
    {
        if (!READ(parser, string)) goto error;
        if (!CACHE(parser, 1)) goto error;
    }

    /* Check if the name is empty. */

    if (string.start == string.pointer) {
        yaml_parser_set_scanner_error(parser, "while scanning a directive",
                start_mark, "could not find expected directive name");
        goto error;
    }

    /* Check for an blank character after the name. */

    if (!IS_BLANKZ(parser->buffer)) {
        yaml_parser_set_scanner_error(parser, "while scanning a directive",
                start_mark, "found unexpected non-alphabetical character");
        goto error;
    }

    *name = string.start;

    return 1;

error:
    STRING_DEL(parser, string);
    return 0;
}