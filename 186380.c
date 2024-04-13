yaml_parser_scan_tag(yaml_parser_t *parser, yaml_token_t *token)
{
    yaml_char_t *handle = NULL;
    yaml_char_t *suffix = NULL;
    yaml_mark_t start_mark, end_mark;

    start_mark = parser->mark;

    /* Check if the tag is in the canonical form. */

    if (!CACHE(parser, 2)) goto error;

    if (CHECK_AT(parser->buffer, '<', 1))
    {
        /* Set the handle to '' */

        handle = yaml_malloc(1);
        if (!handle) goto error;
        handle[0] = '\0';

        /* Eat '!<' */

        SKIP(parser);
        SKIP(parser);

        /* Consume the tag value. */

        if (!yaml_parser_scan_tag_uri(parser, 0, NULL, start_mark, &suffix))
            goto error;

        /* Check for '>' and eat it. */

        if (!CHECK(parser->buffer, '>')) {
            yaml_parser_set_scanner_error(parser, "while scanning a tag",
                    start_mark, "did not find the expected '>'");
            goto error;
        }

        SKIP(parser);
    }
    else
    {
        /* The tag has either the '!suffix' or the '!handle!suffix' form. */

        /* First, try to scan a handle. */

        if (!yaml_parser_scan_tag_handle(parser, 0, start_mark, &handle))
            goto error;

        /* Check if it is, indeed, handle. */

        if (handle[0] == '!' && handle[1] != '\0' && handle[strlen((char *)handle)-1] == '!')
        {
            /* Scan the suffix now. */

            if (!yaml_parser_scan_tag_uri(parser, 0, NULL, start_mark, &suffix))
                goto error;
        }
        else
        {
            /* It wasn't a handle after all.  Scan the rest of the tag. */

            if (!yaml_parser_scan_tag_uri(parser, 0, handle, start_mark, &suffix))
                goto error;

            /* Set the handle to '!'. */

            yaml_free(handle);
            handle = yaml_malloc(2);
            if (!handle) goto error;
            handle[0] = '!';
            handle[1] = '\0';

            /*
             * A special case: the '!' tag.  Set the handle to '' and the
             * suffix to '!'.
             */

            if (suffix[0] == '\0') {
                yaml_char_t *tmp = handle;
                handle = suffix;
                suffix = tmp;
            }
        }
    }

    /* Check the character which ends the tag. */

    if (!CACHE(parser, 1)) goto error;

    if (!IS_BLANKZ(parser->buffer)) {
        yaml_parser_set_scanner_error(parser, "while scanning a tag",
                start_mark, "did not find expected whitespace or line break");
        goto error;
    }

    end_mark = parser->mark;

    /* Create a token. */

    TAG_TOKEN_INIT(*token, handle, suffix, start_mark, end_mark);

    return 1;

error:
    yaml_free(handle);
    yaml_free(suffix);
    return 0;
}