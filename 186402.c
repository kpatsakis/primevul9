yaml_parser_set_scanner_error(yaml_parser_t *parser, const char *context,
        yaml_mark_t context_mark, const char *problem)
{
    parser->error = YAML_SCANNER_ERROR;
    parser->context = context;
    parser->context_mark = context_mark;
    parser->problem = problem;
    parser->problem_mark = parser->mark;

    return 0;
}