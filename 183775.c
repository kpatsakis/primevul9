yang_read_units(struct lys_module *module, void *node, char *value, enum yytokentype type)
{
    int ret;

    switch (type) {
    case LEAF_KEYWORD:
        ret = yang_check_string(module, &((struct lys_node_leaf *) node)->units, "units", "leaf", value, node);
        break;
    case LEAF_LIST_KEYWORD:
        ret = yang_check_string(module, &((struct lys_node_leaflist *) node)->units, "units", "leaflist", value, node);
        break;
    case TYPEDEF_KEYWORD:
        ret = yang_check_string(module, &((struct lys_tpdf *) node)->units, "units", "typedef", value, NULL);
        break;
    case ADD_KEYWORD:
    case REPLACE_KEYWORD:
    case DELETE_KEYWORD:
        ret = yang_check_string(module, &((struct lys_deviate *) node)->units, "units", "deviate", value, NULL);
        break;
    default:
        free(value);
        LOGINT(module->ctx);
        ret = EXIT_FAILURE;
        break;
    }
    return ret;
}