int yang_check_deviate_unique(struct lys_module *module, struct lys_deviate *deviate, struct lys_node *dev_target)
{
    struct lys_node_list *list;
    char *str;
    uint i = 0;
    struct lys_unique *last_unique = NULL;

    if (yang_read_deviate_unique(deviate, dev_target)) {
        goto error;
    }
    list = (struct lys_node_list *)dev_target;
    last_unique = &list->unique[list->unique_size];
    for (i = 0; i < deviate->unique_size; ++i) {
        str = (char *) deviate->unique[i].expr;
        if (deviate->mod == LY_DEVIATE_ADD) {
            if (yang_fill_unique(module, list, &list->unique[list->unique_size], str, NULL)) {
                free(str);
                goto error;
            }
            list->unique_size++;
        } else if (deviate->mod == LY_DEVIATE_DEL) {
            if (yang_fill_unique(module, list, &deviate->unique[i], str, NULL)) {
                free(str);
                goto error;
            }
            if (yang_deviate_delete_unique(module, deviate, list, i, str)) {
                free(str);
                goto error;
            }
        }
        free(str);
    }
    if (deviate->mod == LY_DEVIATE_ADD) {
        free(deviate->unique);
        deviate->unique = last_unique;
    }

    return EXIT_SUCCESS;

error:
    if (deviate->mod == LY_DEVIATE_ADD) {
        for (i = i + 1; i < deviate->unique_size; ++i) {
            free(deviate->unique[i].expr);
        }
        free(deviate->unique);
        deviate->unique = last_unique;

    }
    return EXIT_FAILURE;
}