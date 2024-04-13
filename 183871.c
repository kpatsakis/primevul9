yang_fill_deviate_default(struct ly_ctx *ctx, struct lys_deviate *deviate, struct lys_node *dev_target,
                          struct ly_set *dflt_check, const char *value)
{
    struct lys_node *node;
    struct lys_node_choice *choice;
    struct lys_node_leaf *leaf;
    struct lys_node_leaflist *llist;
    int rc, i, j;
    unsigned int u;

    u = strlen(value);
    if (dev_target->nodetype == LYS_CHOICE) {
        choice = (struct lys_node_choice *)dev_target;
        rc = resolve_choice_default_schema_nodeid(value, choice->child, (const struct lys_node **)&node);
        if (rc || !node) {
            LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "default");
            goto error;
        }
        if (deviate->mod == LY_DEVIATE_DEL) {
            if (!choice->dflt || (choice->dflt != node)) {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "default");
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Value differs from the target being deleted.");
                goto error;
            }
            choice->dflt = NULL;
            /* remove extensions of this default instance from the target node */
            j = -1;
            while ((j = lys_ext_iter(dev_target->ext, dev_target->ext_size, j + 1, LYEXT_SUBSTMT_DEFAULT)) != -1) {
                lyp_ext_instance_rm(ctx, &dev_target->ext, &dev_target->ext_size, j);
                --j;
            }
        } else { /* add or replace */
            choice->dflt = node;
            if (!choice->dflt) {
                /* default branch not found */
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "default");
                goto error;
            }
        }
    } else if (dev_target->nodetype == LYS_LEAF) {
        leaf = (struct lys_node_leaf *)dev_target;
        if (deviate->mod == LY_DEVIATE_DEL) {
            if (!leaf->dflt || !ly_strequal(leaf->dflt, value, 1)) {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "default");
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Value differs from the target being deleted.");
                goto error;
            }
            /* remove value */
            lydict_remove(ctx, leaf->dflt);
            leaf->dflt = NULL;
            leaf->flags &= ~LYS_DFLTJSON;
            /* remove extensions of this default instance from the target node */
            j = -1;
            while ((j = lys_ext_iter(dev_target->ext, dev_target->ext_size, j + 1, LYEXT_SUBSTMT_DEFAULT)) != -1) {
                lyp_ext_instance_rm(ctx, &dev_target->ext, &dev_target->ext_size, j);
                --j;
            }
        } else { /* add (already checked) and replace */
            /* remove value */
            lydict_remove(ctx, leaf->dflt);
            leaf->flags &= ~LYS_DFLTJSON;

            /* set new value */
            leaf->dflt = lydict_insert(ctx, value, u);

            /* remember to check it later (it may not fit now, but the type can be deviated too) */
            ly_set_add(dflt_check, dev_target, 0);
        }
    } else { /* LYS_LEAFLIST */
        llist = (struct lys_node_leaflist *)dev_target;
        if (deviate->mod == LY_DEVIATE_DEL) {
            /* find and remove the value in target list */
            for (i = 0; i < llist->dflt_size; i++) {
                if (llist->dflt[i] && ly_strequal(llist->dflt[i], value, 1)) {
                    /* match, remove the value */
                    lydict_remove(llist->module->ctx, llist->dflt[i]);
                    llist->dflt[i] = NULL;
                    /* remove extensions of this default instance from the target node */
                    j = -1;
                    while ((j = lys_ext_iter(dev_target->ext, dev_target->ext_size, j + 1, LYEXT_SUBSTMT_DEFAULT)) != -1) {
                        if (dev_target->ext[j]->insubstmt_index == i) {
                            lyp_ext_instance_rm(ctx, &dev_target->ext, &dev_target->ext_size, j);
                            --j;
                        } else if (dev_target->ext[j]->insubstmt_index > i) {
                            /* decrease the substatement index of the extension because of the changed array of defaults */
                            dev_target->ext[j]->insubstmt_index--;
                        }
                    }
                    break;
                }
            }
            if (i == llist->dflt_size) {
                LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "default");
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "The default value to delete not found in the target node.");
                goto error;
            }
        } else {
            /* add or replace, anyway we place items into the deviate's list
               which propagates to the target */
            /* we just want to check that the value isn't already in the list */
            for (i = 0; i < llist->dflt_size; i++) {
                if (ly_strequal(llist->dflt[i], value, 1)) {
                    LOGVAL(ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "default");
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Duplicated default value \"%s\".", value);
                    goto error;
                }
            }
            /* store it in target node */
            llist->dflt[llist->dflt_size++] = lydict_insert(ctx, value, u);

            /* remember to check it later (it may not fit now, but the type can be deviated too) */
            ly_set_add(dflt_check, dev_target, 0);
            llist->flags &= ~LYS_DFLTJSON;
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}