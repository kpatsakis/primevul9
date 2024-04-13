yang_read_deviate_unique(struct lys_deviate *deviate, struct lys_node *dev_target)
{
    struct ly_ctx *ctx = dev_target->module->ctx;
    struct lys_node_list *list;
    struct lys_unique *unique;

    /* check target node type */
    if (dev_target->nodetype != LYS_LIST) {
        LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "unique");
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow \"unique\" property.");
        goto error;
    }

    list = (struct lys_node_list *)dev_target;
    if (deviate->mod == LY_DEVIATE_ADD) {
        /* reallocate the unique array of the target */
        unique = ly_realloc(list->unique, (deviate->unique_size + list->unique_size) * sizeof *unique);
        LY_CHECK_ERR_GOTO(!unique, LOGMEM(ctx), error);
        list->unique = unique;
        memset(unique + list->unique_size, 0, deviate->unique_size * sizeof *unique);
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}