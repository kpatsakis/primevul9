yang_deviate_delete_unique(struct lys_module *module, struct lys_deviate *deviate,
                           struct lys_node_list *list, int index, char * value)
{
    struct ly_ctx *ctx = module->ctx;
    int i, j, k;

    /* find unique structures to delete */
    for (i = 0; i < list->unique_size; i++) {
        if (list->unique[i].expr_size != deviate->unique[index].expr_size) {
            continue;
        }

        for (j = 0; j < deviate->unique[index].expr_size; j++) {
            if (!ly_strequal(list->unique[i].expr[j], deviate->unique[index].expr[j], 1)) {
                break;
            }
        }

        if (j == deviate->unique[index].expr_size) {
            /* we have a match, free the unique structure ... */
            for (j = 0; j < list->unique[i].expr_size; j++) {
                lydict_remove(ctx, list->unique[i].expr[j]);
            }
            free(list->unique[i].expr);
            /* ... and maintain the array */
            list->unique_size--;
            if (i != list->unique_size) {
                list->unique[i].expr_size = list->unique[list->unique_size].expr_size;
                list->unique[i].expr = list->unique[list->unique_size].expr;
            }

            if (!list->unique_size) {
                free(list->unique);
                list->unique = NULL;
            } else {
                list->unique[list->unique_size].expr_size = 0;
                list->unique[list->unique_size].expr = NULL;
            }

            k = i; /* remember index for removing extensions */
            i = -1; /* set match flag */
            break;
        }
    }

    if (i != -1) {
        /* no match found */
        LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "unique");
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Value differs from the target being deleted.");
        return EXIT_FAILURE;
    }

    /* remove extensions of this unique instance from the target node */
    j = -1;
    while ((j = lys_ext_iter(list->ext, list->ext_size, j + 1, LYEXT_SUBSTMT_UNIQUE)) != -1) {
        if (list->ext[j]->insubstmt_index == k) {
            lyp_ext_instance_rm(ctx, &list->ext, &list->ext_size, j);
            --j;
        } else if (list->ext[j]->insubstmt_index > k) {
            /* decrease the substatement index of the extension because of the changed array of uniques */
            list->ext[j]->insubstmt_index--;
        }
    }
    return EXIT_SUCCESS;
}