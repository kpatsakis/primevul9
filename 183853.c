yang_read_deviate_default(struct lys_module *module, struct lys_deviate *deviate,
                          struct lys_node *dev_target, struct ly_set * dflt_check)
{
    struct ly_ctx *ctx = module->ctx;
    int i;
    struct lys_node_leaflist *llist;
    const char **dflt;

    /* check target node type */
    if (module->version < 2 && dev_target->nodetype == LYS_LEAFLIST) {
        LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "default");
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow \"default\" property.");
        goto error;
    } else if (deviate->dflt_size > 1 && dev_target->nodetype != LYS_LEAFLIST) { /* from YANG 1.1 */
        LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "default");
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow multiple \"default\" properties.");
        goto error;
    } else if (!(dev_target->nodetype & (LYS_LEAF | LYS_LEAFLIST | LYS_CHOICE))) {
        LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "default");
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow \"default\" property.");
        goto error;
    }

    if (deviate->mod == LY_DEVIATE_ADD) {
        /* check that there is no current value */
        if ((dev_target->nodetype == LYS_LEAF && ((struct lys_node_leaf *)dev_target)->dflt) ||
                (dev_target->nodetype == LYS_CHOICE && ((struct lys_node_choice *)dev_target)->dflt)) {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "default");
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Adding property that already exists.");
            goto error;
        }

        /* check collision with mandatory/min-elements */
        if ((dev_target->flags & LYS_MAND_TRUE) ||
                (dev_target->nodetype == LYS_LEAFLIST && ((struct lys_node_leaflist *)dev_target)->min)) {
            LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, "default", "deviation");
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL,
                   "Adding the \"default\" statement is forbidden on %s statement.",
                   (dev_target->flags & LYS_MAND_TRUE) ? "nodes with the \"mandatory\"" : "leaflists with non-zero \"min-elements\"");
            goto error;
        }
    } else if (deviate->mod == LY_DEVIATE_RPL) {
        /* check that there was a value before */
        if (((dev_target->nodetype & (LYS_LEAF | LYS_LEAFLIST)) && !((struct lys_node_leaf *)dev_target)->dflt) ||
                (dev_target->nodetype == LYS_CHOICE && !((struct lys_node_choice *)dev_target)->dflt)) {
            LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "default");
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Replacing a property that does not exist.");
            goto error;
        }
    }

    if (dev_target->nodetype == LYS_LEAFLIST) {
        /* reallocate default list in the target */
        llist = (struct lys_node_leaflist *)dev_target;
        if (deviate->mod == LY_DEVIATE_ADD) {
            /* reallocate (enlarge) the unique array of the target */
            dflt = realloc(llist->dflt, (deviate->dflt_size + llist->dflt_size) * sizeof *dflt);
            LY_CHECK_ERR_GOTO(!dflt, LOGMEM(ctx), error);
            llist->dflt = dflt;
        } else if (deviate->mod == LY_DEVIATE_RPL) {
            /* reallocate (replace) the unique array of the target */
            for (i = 0; i < llist->dflt_size; i++) {
                lydict_remove(ctx, llist->dflt[i]);
            }
            dflt = realloc(llist->dflt, deviate->dflt_size * sizeof *dflt);
            LY_CHECK_ERR_GOTO(!dflt, LOGMEM(ctx), error);
            llist->dflt = dflt;
            llist->dflt_size = 0;
        }
    }

    for (i = 0; i < deviate->dflt_size; ++i) {
        if (yang_fill_deviate_default(ctx, deviate, dev_target, dflt_check, deviate->dflt[i])) {
            goto error;
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}