yang_parse_mem(struct lys_module *module, struct lys_submodule *submodule, struct unres_schema *unres,
               const char *data, unsigned int size_data, struct lys_node **node)
{
    unsigned int size;
    YY_BUFFER_STATE bp;
    yyscan_t scanner = NULL;
    int ret = 0;
    struct lys_module *trg;
    struct yang_parameter param;

    size = (size_data) ? size_data : strlen(data) + 2;
    yylex_init(&scanner);
    yyset_extra(module->ctx, scanner);
    bp = yy_scan_buffer((char *)data, size, scanner);
    yy_switch_to_buffer(bp, scanner);
    memset(&param, 0, sizeof param);
    param.module = module;
    param.submodule = submodule;
    param.unres = unres;
    param.node = node;
    param.flags |= YANG_REMOVE_IMPORT;
    if (yyparse(scanner, &param)) {
        if (param.flags & YANG_REMOVE_IMPORT) {
            trg = (submodule) ? (struct lys_module *)submodule : module;
            yang_free_import(trg->ctx, trg->imp, 0, trg->imp_size);
            yang_free_include(trg->ctx, trg->inc, 0, trg->inc_size);
            trg->inc_size = 0;
            trg->imp_size = 0;
        }
        ret = (param.flags & YANG_EXIST_MODULE) ? 1 : -1;
    }
    yy_delete_buffer(bp, scanner);
    yylex_destroy(scanner);
    return ret;
}