yang_read_key(struct lys_module *module, struct lys_node_list *list, struct unres_schema *unres)
{
    char *exp, *value;

    exp = value = (char *) list->keys;
    while ((value = strpbrk(value, " \t\n"))) {
        list->keys_size++;
        while (isspace(*value)) {
            value++;
        }
    }
    list->keys_size++;

    list->keys_str = lydict_insert_zc(module->ctx, exp);
    list->keys = calloc(list->keys_size, sizeof *list->keys);
    LY_CHECK_ERR_RETURN(!list->keys, LOGMEM(module->ctx), EXIT_FAILURE);

    if (unres_schema_add_node(module, unres, list, UNRES_LIST_KEYS, NULL) == -1) {
        return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}