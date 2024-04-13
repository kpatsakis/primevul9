yang_read_default(struct lys_module *module, void *node, char *value, enum yytokentype type)
{
    int ret;

    switch (type) {
    case LEAF_KEYWORD:
        ret = yang_check_string(module, &((struct lys_node_leaf *) node)->dflt, "default", "leaf", value, node);
        break;
    case TYPEDEF_KEYWORD:
        ret = yang_check_string(module, &((struct lys_tpdf *) node)->dflt, "default", "typedef", value, NULL);
        break;
    default:
        free(value);
        LOGINT(module->ctx);
        ret = EXIT_FAILURE;
        break;
    }
    return ret;
}