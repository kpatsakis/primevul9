yaml_parser_scan_uri_escapes(yaml_parser_t *parser, int directive,
        yaml_mark_t start_mark, yaml_string_t *string)
{
    int width = 0;

    /* Decode the required number of characters. */

    do {

        unsigned char octet = 0;

        /* Check for a URI-escaped octet. */

        if (!CACHE(parser, 3)) return 0;

        if (!(CHECK(parser->buffer, '%')
                    && IS_HEX_AT(parser->buffer, 1)
                    && IS_HEX_AT(parser->buffer, 2))) {
            return yaml_parser_set_scanner_error(parser, directive ?
                    "while parsing a %TAG directive" : "while parsing a tag",
                    start_mark, "did not find URI escaped octet");
        }

        /* Get the octet. */

        octet = (AS_HEX_AT(parser->buffer, 1) << 4) + AS_HEX_AT(parser->buffer, 2);

        /* If it is the leading octet, determine the length of the UTF-8 sequence. */

        if (!width)
        {
            width = (octet & 0x80) == 0x00 ? 1 :
                    (octet & 0xE0) == 0xC0 ? 2 :
                    (octet & 0xF0) == 0xE0 ? 3 :
                    (octet & 0xF8) == 0xF0 ? 4 : 0;
            if (!width) {
                return yaml_parser_set_scanner_error(parser, directive ?
                        "while parsing a %TAG directive" : "while parsing a tag",
                        start_mark, "found an incorrect leading UTF-8 octet");
            }
        }
        else
        {
            /* Check if the trailing octet is correct. */

            if ((octet & 0xC0) != 0x80) {
                return yaml_parser_set_scanner_error(parser, directive ?
                        "while parsing a %TAG directive" : "while parsing a tag",
                        start_mark, "found an incorrect trailing UTF-8 octet");
            }
        }

        /* Copy the octet and move the pointers. */

        *(string->pointer++) = octet;
        SKIP(parser);
        SKIP(parser);
        SKIP(parser);

    } while (--width);

    return 1;
}