yang_check_deviate(struct lys_module *module, struct unres_schema *unres, struct lys_deviate *deviate,
                   struct lys_node *dev_target, struct ly_set *dflt_check)
{
    struct lys_node_leaflist *llist;
    struct lys_type *type;
    struct lys_tpdf *tmp_parent;
    int i, j;

    if (yang_check_ext_instance(module, &deviate->ext, deviate->ext_size, deviate, unres)) {
        goto error;
    }
    if (deviate->must_size && yang_check_deviate_must(module, unres, deviate, dev_target)) {
        goto error;
    }
    if (deviate->unique && yang_check_deviate_unique(module, deviate, dev_target)) {
        goto error;
    }
    if (deviate->dflt_size) {
        if (yang_read_deviate_default(module, deviate, dev_target, dflt_check)) {
            goto error;
        }
        if (dev_target->nodetype == LYS_LEAFLIST && deviate->mod == LY_DEVIATE_DEL) {
            /* consolidate the final list in the target after removing items from it */
            llist = (struct lys_node_leaflist *)dev_target;
            for (i = j = 0; j < llist->dflt_size; j++) {
                llist->dflt[i] = llist->dflt[j];
                if (llist->dflt[i]) {
                    i++;
                }
            }
            llist->dflt_size = i + 1;
        }
    }

    if (deviate->max_set && yang_read_deviate_minmax(deviate, dev_target, deviate->max, 1)) {
        goto error;
    }

    if (deviate->min_set && yang_read_deviate_minmax(deviate, dev_target, deviate->min, 0)) {
        goto error;
    }

    if (deviate->units && yang_read_deviate_units(module->ctx, deviate, dev_target)) {
        goto error;
    }

    if ((deviate->flags & LYS_CONFIG_MASK)) {
        /* add and replace are the same in this case */
        /* remove current config value of the target ... */
        dev_target->flags &= ~LYS_CONFIG_MASK;

        /* ... and replace it with the value specified in deviation */
        dev_target->flags |= deviate->flags & LYS_CONFIG_MASK;
    }

    if ((deviate->flags & LYS_MAND_MASK) && yang_check_deviate_mandatory(deviate, dev_target)) {
        goto error;
    }

    if (deviate->type) {
        /* check target node type */
        if (dev_target->nodetype == LYS_LEAF) {
            type = &((struct lys_node_leaf *)dev_target)->type;
        } else if (dev_target->nodetype == LYS_LEAFLIST) {
            type = &((struct lys_node_leaflist *)dev_target)->type;
        } else {
            LOGVAL(module->ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "type");
            LOGVAL(module->ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Target node does not allow \"type\" property.");
            goto error;
        }
        /* remove type and initialize it */
        tmp_parent = type->parent;
        lys_type_free(module->ctx, type, NULL);
        memcpy(type, deviate->type, sizeof *deviate->type);
        free(deviate->type);
        deviate->type = type;
        deviate->type->parent = tmp_parent;
        if (yang_fill_type(module, type, (struct yang_type *)type->der, tmp_parent, unres)) {
            goto error;
        }

        if (unres_schema_add_node(module, unres, deviate->type, UNRES_TYPE_DER, dev_target) == -1) {
            goto error;
        }
    }

    return EXIT_SUCCESS;

error:
    if (deviate->type) {
        yang_type_free(module->ctx, deviate->type);
        deviate->type = NULL;
    }
    return EXIT_FAILURE;
}