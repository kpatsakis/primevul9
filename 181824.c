lyp_rfn_apply_ext(struct lys_module *module)
{
    int i, k, a = 0;
    struct lys_node *root, *nextroot, *next, *node;
    struct lys_node *target;
    struct lys_node_uses *uses;
    struct lys_refine *rfn;
    struct ly_set *extset;

    /* refines in uses */
    LY_TREE_FOR_SAFE(module->data, nextroot, root) {
        /* go through the data tree of the module and all the defined augments */

        LY_TREE_DFS_BEGIN(root, next, node) {
            if (node->nodetype == LYS_USES) {
                uses = (struct lys_node_uses *)node;

                for (i = 0; i < uses->refine_size; i++) {
                    if (!uses->refine[i].ext_size) {
                        /* no extensions in refine */
                        continue;
                    }
                    rfn = &uses->refine[i]; /* shortcut */

                    /* get the target node */
                    target = NULL;
                    resolve_descendant_schema_nodeid(rfn->target_name, uses->child,
                                                     LYS_NO_RPC_NOTIF_NODE | LYS_ACTION | LYS_NOTIF,
                                                     0, (const struct lys_node **)&target);
                    if (!target) {
                        /* it should always succeed since the target_name was already resolved at least
                         * once when the refine itself was being resolved */
                        LOGINT(module->ctx);;
                        return EXIT_FAILURE;
                    }

                    /* extensions */
                    extset = ly_set_new();
                    k = -1;
                    while ((k = lys_ext_iter(rfn->ext, rfn->ext_size, k + 1, LYEXT_SUBSTMT_SELF)) != -1) {
                        ly_set_add(extset, rfn->ext[k]->def, 0);
                    }
                    for (k = 0; (unsigned int)k < extset->number; k++) {
                        if (lyp_rfn_apply_ext_(rfn, target, LYEXT_SUBSTMT_SELF, (struct lys_ext *)extset->set.g[k])) {
                            ly_set_free(extset);
                            return EXIT_FAILURE;
                        }
                    }
                    ly_set_free(extset);

                    /* description */
                    if (rfn->dsc && lyp_rfn_apply_ext_(rfn, target, LYEXT_SUBSTMT_DESCRIPTION, NULL)) {
                        return EXIT_FAILURE;
                    }
                    /* reference */
                    if (rfn->ref && lyp_rfn_apply_ext_(rfn, target, LYEXT_SUBSTMT_REFERENCE, NULL)) {
                        return EXIT_FAILURE;
                    }
                    /* config, in case of notification or rpc/action{notif, the config is not applicable
                     * (there is no config status) */
                    if ((rfn->flags & LYS_CONFIG_MASK) && (target->flags & LYS_CONFIG_MASK)) {
                        if (lyp_rfn_apply_ext_(rfn, target, LYEXT_SUBSTMT_CONFIG, NULL)) {
                            return EXIT_FAILURE;
                        }
                    }
                    /* default value */
                    if (rfn->dflt_size && lyp_rfn_apply_ext_(rfn, target, LYEXT_SUBSTMT_DEFAULT, NULL)) {
                        return EXIT_FAILURE;
                    }
                    /* mandatory */
                    if (rfn->flags & LYS_MAND_MASK) {
                        if (lyp_rfn_apply_ext_(rfn, target, LYEXT_SUBSTMT_MANDATORY, NULL)) {
                            return EXIT_FAILURE;
                        }
                    }
                    /* presence */
                    if ((target->nodetype & LYS_CONTAINER) && rfn->mod.presence) {
                        if (lyp_rfn_apply_ext_(rfn, target, LYEXT_SUBSTMT_PRESENCE, NULL)) {
                            return EXIT_FAILURE;
                        }
                    }
                    /* min/max */
                    if (rfn->flags & LYS_RFN_MINSET) {
                        if (lyp_rfn_apply_ext_(rfn, target, LYEXT_SUBSTMT_MIN, NULL)) {
                            return EXIT_FAILURE;
                        }
                    }
                    if (rfn->flags & LYS_RFN_MAXSET) {
                        if (lyp_rfn_apply_ext_(rfn, target, LYEXT_SUBSTMT_MAX, NULL)) {
                            return EXIT_FAILURE;
                        }
                    }
                    /* must and if-feature contain extensions on their own, not needed to be solved here */

                    if (target->ext_size) {
                        /* the allocated target's extension array can be now longer than needed in case
                         * there is less refine substatement's extensions than in original. Since we are
                         * going to reduce or keep the same memory, it is not necessary to test realloc's result */
                        target->ext = realloc(target->ext, target->ext_size * sizeof *target->ext);
                    }
                }
            }
            LY_TREE_DFS_END(root, next, node)
        }

        if (!nextroot && a < module->augment_size) {
            nextroot = module->augment[a].child;
            a++;
        }
    }

    return EXIT_SUCCESS;
}