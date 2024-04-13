yang_read_reference(struct lys_module *module, void *node, char *value, char *where, enum yytokentype type)
{
    int ret;
    char *ref = "reference";

    switch (type) {
    case MODULE_KEYWORD:
        ret = yang_check_string(module, &module->ref, ref, "module", value, NULL);
        break;
    case REVISION_KEYWORD:
        ret = yang_check_string(module, &((struct lys_revision *)node)->ref, ref, where, value, NULL);
        break;
    case IMPORT_KEYWORD:
        ret = yang_check_string(module, &((struct lys_import *)node)->ref, ref, where, value, NULL);
        break;
    case INCLUDE_KEYWORD:
        ret = yang_check_string(module, &((struct lys_include *)node)->ref, ref, where, value, NULL);
        break;
    case NODE_PRINT:
        ret = yang_check_string(module, &((struct lys_node *)node)->ref, ref, where, value, node);
        break;
    default:
        ret = yang_check_string(module, &((struct lys_node *)node)->ref, ref, where, value, NULL);
        break;
    }
    return ret;
}