yaml_parser_remove_simple_key(yaml_parser_t *parser)
{
    yaml_simple_key_t *simple_key = parser->simple_keys.top-1;

    if (simple_key->possible)
    {
        /* If the key is required, it is an error. */

        if (simple_key->required) {
            return yaml_parser_set_scanner_error(parser,
                    "while scanning a simple key", simple_key->mark,
                    "could not find expected ':'");
        }
    }

    /* Remove the key from the stack. */

    simple_key->possible = 0;

    return 1;
}