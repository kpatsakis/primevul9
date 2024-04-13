lyp_deviate_apply_ext(struct lys_deviate *dev, struct lys_node *target, LYEXT_SUBSTMT substmt, struct lys_ext *extdef)
{
    struct ly_ctx *ctx;
    int m, n;
    struct lys_ext_instance *new;
    void *reallocated;

    /* LY_DEVIATE_ADD and LY_DEVIATE_RPL are very similar so they are implement the same way - in replacing,
     * there can be some extension instances in the target, in case of adding, there should not be any so we
     * will be just adding. */

    ctx = target->module->ctx; /* shortcut */
    m = n = -1;

    while ((m = lys_ext_iter(dev->ext, dev->ext_size, m + 1, substmt)) != -1) {
        /* deviate and its substatements include extensions, copy them to the target, replacing the previous
         * extensions if any. In case of deviating extension itself, we have to deviate only the same type
         * of the extension as specified in the deviation */
        if (substmt == LYEXT_SUBSTMT_SELF && dev->ext[m]->def != extdef) {
            continue;
        }

        if (substmt == LYEXT_SUBSTMT_SELF && dev->mod == LY_DEVIATE_ADD) {
            /* in case of adding extension, we will be replacing only the inherited extensions */
            do {
                n = lys_ext_iter(target->ext, target->ext_size, n + 1, substmt);
            } while (n != -1 && (target->ext[n]->def != extdef || !(target->ext[n]->flags & LYEXT_OPT_INHERIT)));
        } else {
            /* get the index of the extension to replace in the target node */
            do {
                n = lys_ext_iter(target->ext, target->ext_size, n + 1, substmt);
                /* if we are applying extension deviation, we have to deviate only the same type of the extension */
            } while (n != -1 && substmt == LYEXT_SUBSTMT_SELF && target->ext[n]->def != extdef);
        }

        if (n == -1) {
            /* nothing to replace, we are going to add it - reallocate */
            new = malloc(sizeof **target->ext);
            LY_CHECK_ERR_RETURN(!new, LOGMEM(ctx), EXIT_FAILURE);
            reallocated = realloc(target->ext, (target->ext_size + 1) * sizeof *target->ext);
            LY_CHECK_ERR_RETURN(!reallocated, LOGMEM(ctx); free(new), EXIT_FAILURE);
            target->ext = reallocated;
            target->ext_size++;

            n = target->ext_size - 1;
        } else {
            /* replacing - the original set of extensions is actually backuped together with the
             * node itself, so we are supposed only to free the allocated data here ... */
            lys_extension_instances_free(ctx, target->ext[n]->ext, target->ext[n]->ext_size, NULL);
            lydict_remove(ctx, target->ext[n]->arg_value);
            free(target->ext[n]);

            /* and prepare the new structure */
            new = malloc(sizeof **target->ext);
            LY_CHECK_ERR_RETURN(!new, LOGMEM(ctx), EXIT_FAILURE);
        }
        /* common part for adding and replacing - fill the newly created / replaced cell */
        target->ext[n] = new;
        target->ext[n]->def = dev->ext[m]->def;
        target->ext[n]->arg_value = lydict_insert(ctx, dev->ext[m]->arg_value, 0);
        target->ext[n]->flags = 0;
        target->ext[n]->parent = target;
        target->ext[n]->parent_type = LYEXT_PAR_NODE;
        target->ext[n]->insubstmt = substmt;
        target->ext[n]->insubstmt_index = dev->ext[m]->insubstmt_index;
        target->ext[n]->ext_size = dev->ext[m]->ext_size;
        lys_ext_dup(ctx, target->module, dev->ext[m]->ext, dev->ext[m]->ext_size, target, LYEXT_PAR_NODE,
                    &target->ext[n]->ext, 1, NULL);
        target->ext[n]->nodetype = LYS_EXT;
        target->ext[n]->module = target->module;
        target->ext[n]->priv = NULL;

        /* TODO cover complex extension instances */
    }

    /* remove the rest of extensions belonging to the original substatement in the target node,
     * due to possible reverting of the deviation effect, they are actually not removed, just moved
     * to the backup of the original node when the original node is backuped, here we just have to
     * free the replaced / deleted originals */
    while ((n = lys_ext_iter(target->ext, target->ext_size, n + 1, substmt)) != -1) {
        if (substmt == LYEXT_SUBSTMT_SELF) {
            /* if we are applying extension deviation, we are going to remove only
             * - the same type of the extension in case of replacing
             * - the same type of the extension which was inherited in case of adding
             * note - delete deviation is covered in lyp_deviate_del_ext */
            if (target->ext[n]->def != extdef ||
                    (dev->mod == LY_DEVIATE_ADD && !(target->ext[n]->flags & LYEXT_OPT_INHERIT))) {
                /* keep this extension */
                continue;
            }

        }

        /* remove the item */
        lyp_ext_instance_rm(ctx, &target->ext, &target->ext_size, n);
        --n;
    }

    return EXIT_SUCCESS;
}