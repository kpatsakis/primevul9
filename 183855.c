yang_parse_ext_substatement(struct lys_module *module, struct unres_schema *unres, const char *data,
                            char *ext_name, struct lys_ext_instance_complex *ext)
{
    unsigned int size;
    YY_BUFFER_STATE bp;
    yyscan_t scanner = NULL;
    int ret = 0;
    struct yang_parameter param;
    struct lys_node *node = NULL;

    if (!data) {
        return EXIT_SUCCESS;
    }
    size = strlen(data) + 2;
    yylex_init(&scanner);
    bp = yy_scan_buffer((char *)data, size, scanner);
    yy_switch_to_buffer(bp, scanner);
    memset(&param, 0, sizeof param);
    param.module = module;
    param.unres = unres;
    param.node = &node;
    param.actual_node = (void **)ext;
    param.data_node = (void **)ext_name;
    param.flags |= EXT_INSTANCE_SUBSTMT;
    if (yyparse(scanner, &param)) {
        yang_free_nodes(module->ctx, node);
        ret = -1;
    } else {
        /* success parse, but it needs some sematic controls */
        if (node && yang_check_nodes(module, (struct lys_node *)ext, node, LYS_PARSE_OPT_CFG_NOINHERIT, unres)) {
            ret = -1;
        }
    }
    yy_delete_buffer(bp, scanner);
    yylex_destroy(scanner);
    return ret;
}