yang_read_deviate_units(struct ly_ctx *ctx, struct lys_deviate *deviate, struct lys_node *dev_target)
{
    const char **stritem;
    int j;

    /* check target node type */
    if (dev_target->nodetype == LYS_LEAFLIST) {
        stritem = &((struct lys_node_leaflist *)dev_target)->units;
    } else if (dev_target->nodetype == LYS_LEAF) {
        stritem = &((struct lys_node_leaf *)dev_target)->units;
    } else {
        LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "units");
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow \"units\" property.");
        goto error;
    }

    if (deviate->mod == LY_DEVIATE_DEL) {
        /* check values */
        if (!ly_strequal(*stritem, deviate->units, 1)) {
            LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, deviate->units, "units");
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Value differs from the target being deleted.");
            goto error;
        }
        /* remove current units value of the target */
        lydict_remove(ctx, *stritem);
        *stritem = NULL;
        /* remove its extensions */
        j = -1;
        while ((j = lys_ext_iter(dev_target->ext, dev_target->ext_size, j + 1, LYEXT_SUBSTMT_UNITS)) != -1) {
            lyp_ext_instance_rm(ctx, &dev_target->ext, &dev_target->ext_size, j);
            --j;
        }
    } else {
        if (deviate->mod == LY_DEVIATE_ADD) {
            /* check that there is no current value */
            if (*stritem) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "units");
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Adding property that already exists.");
                goto error;
            }
        } else { /* replace */
            if (!*stritem) {
                LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "units");
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Replacing a property that does not exist.");
                goto error;
            }
        }
        /* remove current units value of the target ... */
        lydict_remove(ctx, *stritem);

        /* ... and replace it with the value specified in deviation */
        *stritem = lydict_insert(ctx, deviate->units, 0);
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}