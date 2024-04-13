yaml_parser_stale_simple_keys(yaml_parser_t *parser)
{
    yaml_simple_key_t *simple_key;

    /* Check for a potential simple key for each flow level. */

    for (simple_key = parser->simple_keys.start;
            simple_key != parser->simple_keys.top; simple_key ++)
    {
        /*
         * The specification requires that a simple key
         *
         *  - is limited to a single line,
         *  - is shorter than 1024 characters.
         */

        if (simple_key->possible
                && (simple_key->mark.line < parser->mark.line
                    || simple_key->mark.index+1024 < parser->mark.index)) {

            /* Check if the potential simple key to be removed is required. */

            if (simple_key->required) {
                return yaml_parser_set_scanner_error(parser,
                        "while scanning a simple key", simple_key->mark,
                        "could not find expected ':'");
            }

            simple_key->possible = 0;
        }
    }

    return 1;
}