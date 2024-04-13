yang_read_deviate_minmax(struct lys_deviate *deviate, struct lys_node *dev_target, uint32_t value, int type)
{
    struct ly_ctx *ctx = dev_target->module->ctx;
    uint32_t *ui32val, *min, *max;

    /* check target node type */
    if (dev_target->nodetype == LYS_LEAFLIST) {
        max = &((struct lys_node_leaflist *)dev_target)->max;
        min = &((struct lys_node_leaflist *)dev_target)->min;
    } else if (dev_target->nodetype == LYS_LIST) {
        max = &((struct lys_node_list *)dev_target)->max;
        min = &((struct lys_node_list *)dev_target)->min;
    } else {
        LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, (type) ? "max-elements" : "min-elements");
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow \"%s\" property.", (type) ? "max-elements" : "min-elements");
        goto error;
    }

    ui32val = (type) ? max : min;
    if (deviate->mod == LY_DEVIATE_ADD) {
        /* check that there is no current value */
        if (*ui32val) {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, (type) ? "max-elements" : "min-elements");
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Adding property that already exists.");
            goto error;
        }
    } else if (deviate->mod == LY_DEVIATE_RPL) {
        /* unfortunately, there is no way to check reliably that there
         * was a value before, it could have been the default */
    }

    /* add (already checked) and replace */
    /* set new value specified in deviation */
    *ui32val = value;

    /* check min-elements is smaller than max-elements */
    if (*max && *min > *max) {
        if (type) {
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid value \"%d\" of \"max-elements\".", value);
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "\"max-elements\" is smaller than \"min-elements\".");
        } else {
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Invalid value \"%d\" of \"min-elements\".", value);
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "\"min-elements\" is bigger than \"max-elements\".");
        }
        goto error;
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}