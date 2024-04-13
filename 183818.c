yang_check_deviation(struct lys_module *module, struct unres_schema *unres, struct lys_deviation *dev)
{
    int rc;
    uint i;
    struct lys_node *dev_target = NULL, *parent;
    struct ly_set *dflt_check = ly_set_new(), *set;
    unsigned int u;
    const char *value, *target_name;
    struct lys_node_leaflist *llist;
    struct lys_node_leaf *leaf;
    struct lys_node_inout *inout;
    struct unres_schema tmp_unres;
    struct lys_module *mod;

    /* resolve target node */
    rc = resolve_schema_nodeid(dev->target_name, NULL, module, &set, 0, 1);
    if (rc == -1) {
        LOGVAL(module->ctx, LYE_INARG, LY_VLOG_NONE, NULL, dev->target_name, "deviation");
        ly_set_free(set);
        i = 0;
        goto free_type_error;
    }
    dev_target = set->set.s[0];
    ly_set_free(set);

    if (dev_target->module == lys_main_module(module)) {
        LOGVAL(module->ctx, LYE_INARG, LY_VLOG_NONE, NULL, dev->target_name, "deviation");
        LOGVAL(module->ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Deviating own module is not allowed.");
        i = 0;
        goto free_type_error;
    }

    if (!dflt_check) {
        LOGMEM(module->ctx);
        i = 0;
        goto free_type_error;
    }

    if (dev->deviate[0].mod == LY_DEVIATE_NO) {
        /* you cannot remove a key leaf */
        if ((dev_target->nodetype == LYS_LEAF) && dev_target->parent && (dev_target->parent->nodetype == LYS_LIST)) {
            for (i = 0; i < ((struct lys_node_list *)dev_target->parent)->keys_size; ++i) {
                if (((struct lys_node_list *)dev_target->parent)->keys[i] == (struct lys_node_leaf *)dev_target) {
                    LOGVAL(module->ctx, LYE_INARG, LY_VLOG_NONE, NULL, "not-supported", "deviation");
                    LOGVAL(module->ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "\"not-supported\" deviation cannot remove a list key.");
                    i = 0;
                    goto free_type_error;
                }
            }
        }
        /* unlink and store the original node */
        parent = dev_target->parent;
        lys_node_unlink(dev_target);
        if (parent) {
            if (parent->nodetype & (LYS_AUGMENT | LYS_USES)) {
                /* hack for augment, because when the original will be sometime reconnected back, we actually need
                 * to reconnect it to both - the augment and its target (which is deduced from the deviations target
                 * path), so we need to remember the augment as an addition */
                /* remember uses parent so we can reconnect to it */
                dev_target->parent = parent;
            } else if (parent->nodetype & (LYS_RPC | LYS_ACTION)) {
                /* re-create implicit node */
                inout = calloc(1, sizeof *inout);
                LY_CHECK_ERR_GOTO(!inout, LOGMEM(module->ctx), error);

                inout->nodetype = dev_target->nodetype;
                inout->name = lydict_insert(module->ctx, (inout->nodetype == LYS_INPUT) ? "input" : "output", 0);
                inout->module = dev_target->module;
                inout->flags = LYS_IMPLICIT;

                /* insert it manually */
                assert(parent->child && !parent->child->next
                    && (parent->child->nodetype == (inout->nodetype == LYS_INPUT ? LYS_OUTPUT : LYS_INPUT)));
                parent->child->next = (struct lys_node *)inout;
                inout->prev = parent->child;
                parent->child->prev = (struct lys_node *)inout;
                inout->parent = parent;
            }
        }
        dev->orig_node = dev_target;
    } else {
        /* store a shallow copy of the original node */
        memset(&tmp_unres, 0, sizeof tmp_unres);
        dev->orig_node = lys_node_dup(dev_target->module, NULL, dev_target, &tmp_unres, 1);
        /* just to be safe */
        if (tmp_unres.count) {
            LOGINT(module->ctx);
            i = 0;
            goto free_type_error;
        }
    }

    if (yang_check_ext_instance(module, &dev->ext, dev->ext_size, dev, unres)) {
        i = 0;
        goto free_type_error;
    }

    for (i = 0; i < dev->deviate_size; ++i) {
        if (yang_check_deviate(module, unres, &dev->deviate[i], dev_target, dflt_check)) {
            yang_free_deviate(module->ctx, dev, i + 1);
            dev->deviate_size = i + 1;
            goto free_type_error;
        }
    }
    /* now check whether default value, if any, matches the type */
    for (u = 0; u < dflt_check->number; ++u) {
        value = NULL;
        rc = EXIT_SUCCESS;
        if (dflt_check->set.s[u]->nodetype == LYS_LEAF) {
            leaf = (struct lys_node_leaf *)dflt_check->set.s[u];
            target_name = leaf->name;
            value = leaf->dflt;
            rc = unres_schema_add_node(module, unres, &leaf->type, UNRES_TYPE_DFLT, (struct lys_node *)(&leaf->dflt));
        } else { /* LYS_LEAFLIST */
            llist = (struct lys_node_leaflist *)dflt_check->set.s[u];
            target_name = llist->name;
            for (i = 0; i < llist->dflt_size; i++) {
                rc = unres_schema_add_node(module, unres, &llist->type, UNRES_TYPE_DFLT,
                                           (struct lys_node *)(&llist->dflt[i]));
                if (rc == -1) {
                    value = llist->dflt[i];
                    break;
                }
            }
        }
        if (rc == -1) {
            LOGVAL(module->ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "default");
            LOGVAL(module->ctx, LYE_SPEC, LY_VLOG_NONE, NULL,
                "The default value \"%s\" of the deviated node \"%s\"no longer matches its type.",
                target_name);
            goto error;
        }
    }
    ly_set_free(dflt_check);
    dflt_check = NULL;

    /* mark all the affected modules as deviated and implemented */
    for (parent = dev_target; parent; parent = lys_parent(parent)) {
        mod = lys_node_module(parent);
        if (module != mod) {
            mod->deviated = 1;            /* main module */
            parent->module->deviated = 1; /* possible submodule */
            if (!mod->implemented) {
                mod->implemented = 1;
                if (unres_schema_add_node(mod, unres, NULL, UNRES_MOD_IMPLEMENT, NULL) == -1) {
                    goto error;
                }
            }
        }
    }

    return EXIT_SUCCESS;

free_type_error:
    /* we need to free types because they are for now allocated dynamically (use i as it is now, is set correctly) */
    for (; i < dev->deviate_size; ++i) {
        if (dev->deviate[i].type) {
            yang_type_free(module->ctx, dev->deviate[i].type);
            free(dev->deviate[i].type);
            dev->deviate[i].type = NULL;
        }
    }
error:
    ly_set_free(dflt_check);
    return EXIT_FAILURE;
}