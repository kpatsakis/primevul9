yang_read_unique(struct lys_module *module, struct lys_node_list *list, struct unres_schema *unres)
{
    uint8_t k;
    char *str;

    for (k = 0; k < list->unique_size; k++) {
        str = (char *)list->unique[k].expr;
        if (yang_fill_unique(module, list, &list->unique[k], str, unres)) {
            goto error;
        }
        free(str);
    }
    return EXIT_SUCCESS;

error:
    free(str);
    return EXIT_FAILURE;
}