yaml_parser_scan_directive(yaml_parser_t *parser, yaml_token_t *token)
{
    yaml_mark_t start_mark, end_mark;
    yaml_char_t *name = NULL;
    int major, minor;
    yaml_char_t *handle = NULL, *prefix = NULL;

    /* Eat '%'. */

    start_mark = parser->mark;

    SKIP(parser);

    /* Scan the directive name. */

    if (!yaml_parser_scan_directive_name(parser, start_mark, &name))
        goto error;

    /* Is it a YAML directive? */

    if (strcmp((char *)name, "YAML") == 0)
    {
        /* Scan the VERSION directive value. */

        if (!yaml_parser_scan_version_directive_value(parser, start_mark,
                    &major, &minor))
            goto error;

        end_mark = parser->mark;

        /* Create a VERSION-DIRECTIVE token. */

        VERSION_DIRECTIVE_TOKEN_INIT(*token, major, minor,
                start_mark, end_mark);
    }

    /* Is it a TAG directive? */

    else if (strcmp((char *)name, "TAG") == 0)
    {
        /* Scan the TAG directive value. */

        if (!yaml_parser_scan_tag_directive_value(parser, start_mark,
                    &handle, &prefix))
            goto error;

        end_mark = parser->mark;

        /* Create a TAG-DIRECTIVE token. */

        TAG_DIRECTIVE_TOKEN_INIT(*token, handle, prefix,
                start_mark, end_mark);
    }

    /* Unknown directive. */

    else
    {
        yaml_parser_set_scanner_error(parser, "while scanning a directive",
                start_mark, "found uknown directive name");
        goto error;
    }

    /* Eat the rest of the line including any comments. */

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
        yaml_parser_set_scanner_error(parser, "while scanning a directive",
                start_mark, "did not find expected comment or line break");
        goto error;
    }

    /* Eat a line break. */

    if (IS_BREAK(parser->buffer)) {
        if (!CACHE(parser, 2)) goto error;
        SKIP_LINE(parser);
    }

    yaml_free(name);

    return 1;

error:
    yaml_free(prefix);
    yaml_free(handle);
    yaml_free(name);
    return 0;
}