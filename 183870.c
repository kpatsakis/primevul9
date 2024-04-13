yang_read_description(struct lys_module *module, void *node, char *value, char *where, enum yytokentype type)
{
    int ret;
    char *dsc = "description";

    switch (type) {
    case MODULE_KEYWORD:
        ret = yang_check_string(module, &module->dsc, dsc, "module", value, NULL);
        break;
    case REVISION_KEYWORD:
        ret = yang_check_string(module, &((struct lys_revision *)node)->dsc, dsc, where, value, NULL);
        break;
    case IMPORT_KEYWORD:
        ret = yang_check_string(module, &((struct lys_import *)node)->dsc, dsc, where, value, NULL);
        break;
    case INCLUDE_KEYWORD:
        ret = yang_check_string(module, &((struct lys_include *)node)->dsc, dsc, where, value, NULL);
        break;
    case NODE_PRINT:
        ret = yang_check_string(module, &((struct lys_node *)node)->dsc, dsc, where, value, node);
        break;
    default:
        ret = yang_check_string(module, &((struct lys_node *)node)->dsc, dsc, where, value, NULL);
        break;
    }
    return ret;
}