yaml_parser_decrease_flow_level(yaml_parser_t *parser)
{
    yaml_simple_key_t dummy_key;    /* Used to eliminate a compiler warning. */

    if (parser->flow_level) {
        parser->flow_level --;
        dummy_key = POP(parser, parser->simple_keys);
    }

    return 1;
}