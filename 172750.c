resolve_schema_leafref(struct lys_type *type, struct lys_node *parent, struct unres_schema *unres)
{
    const struct lys_node *node, *op_node = NULL, *tmp_parent;
    struct ly_set *node_set;
    struct lys_node_augment *last_aug;
    const struct lys_module *tmp_mod, *cur_module;
    const char *id, *prefix, *name;
    int pref_len, nam_len, parent_times, has_predicate;
    int i, first_iter;
    struct ly_ctx *ctx = parent->module->ctx;

    first_iter = 1;
    parent_times = 0;
    id = type->info.lref.path;
    node_set = ly_set_new();
    if (!node_set) {
        LOGMEM(ctx);
        return -1;
    }

    /* find operation schema we are in */
    for (op_node = lys_parent(parent);
        op_node && !(op_node->nodetype & (LYS_ACTION | LYS_NOTIF | LYS_RPC));
        op_node = lys_parent(op_node));

    cur_module = lys_node_module(parent);
    do {
        if ((i = parse_path_arg(cur_module, id, &prefix, &pref_len, &name, &nam_len, &parent_times, &has_predicate)) < 1) {
            LOGVAL(ctx, LYE_INCHAR, LY_VLOG_LYS, parent, id[-i], &id[-i]);
            ly_set_free(node_set);
            return -1;
        }
        id += i;

        /* get the current module */
        tmp_mod = prefix ? lyp_get_module(cur_module, NULL, 0, prefix, pref_len, 0) : cur_module;
        if (!tmp_mod) {
            LOGVAL(ctx, LYE_NORESOLV, LY_VLOG_LYS, parent, "leafref", type->info.lref.path);
            ly_set_free(node_set);
            return EXIT_FAILURE;
        }
        last_aug = NULL;

        if (first_iter) {
            if (parent_times == -1) {
                /* use module data */
                node = NULL;

            } else if (parent_times > 0) {
                /* we are looking for the right parent */
                for (i = 0, node = parent; i < parent_times; i++) {
                    if (node->parent && (node->parent->nodetype == LYS_AUGMENT)
                            && !((struct lys_node_augment *)node->parent)->target) {
                        /* we are in an unresolved augment, cannot evaluate */
                        LOGVAL(ctx, LYE_SPEC, LY_VLOG_LYS, node->parent,
                            "Cannot resolve leafref \"%s\" because it is in an unresolved augment.", type->info.lref.path);
                        ly_set_free(node_set);
                        return EXIT_FAILURE;
                    }

                    /* path is supposed to be evaluated in data tree, so we have to skip
                     * all schema nodes that cannot be instantiated in data tree */
                    for (node = lys_parent(node);
                        node && !(node->nodetype & (LYS_CONTAINER | LYS_LIST | LYS_ACTION | LYS_NOTIF | LYS_RPC));
                        node = lys_parent(node));

                    if (!node) {
                        if (i == parent_times - 1) {
                            /* top-level */
                            break;
                        }

                        /* higher than top-level */
                        LOGVAL(ctx, LYE_NORESOLV, LY_VLOG_LYS, parent, "leafref", type->info.lref.path);
                        ly_set_free(node_set);
                        return EXIT_FAILURE;
                    }
                }
            } else {
                LOGINT(ctx);
                ly_set_free(node_set);
                return -1;
            }
        }

        /* find the next node (either in unconnected augment or as a schema sibling, node is NULL for top-level node -
         * - useless to search for that in augments) */
        if (!tmp_mod->implemented && node) {
    get_next_augment:
            last_aug = lys_getnext_target_aug(last_aug, tmp_mod, node);
        }

        tmp_parent = (last_aug ? (struct lys_node *)last_aug : node);
        node = NULL;
        while ((node = lys_getnext(node, tmp_parent, tmp_mod, LYS_GETNEXT_NOSTATECHECK))) {
            if (lys_node_module(node) != lys_main_module(tmp_mod)) {
                continue;
            }
            if (strncmp(node->name, name, nam_len) || node->name[nam_len]) {
                continue;
            }
            /* match */
            break;
        }
        if (!node) {
            if (last_aug) {
                /* restore the correct augment target */
                node = last_aug->target;
                goto get_next_augment;
            }
            LOGVAL(ctx, LYE_NORESOLV, LY_VLOG_LYS, parent, "leafref", type->info.lref.path);
            ly_set_free(node_set);
            return EXIT_FAILURE;
        }

        if (first_iter) {
            /* set external dependency flag, we can decide based on the first found node */
            if (resolve_schema_leafref_valid_dep_flag(op_node, cur_module, node, (parent_times == -1 ? 1 : 0))) {
                parent->flags |= LYS_LEAFREF_DEP;
            }
            first_iter = 0;
        }

        if (has_predicate) {
            /* we have predicate, so the current result must be list */
            if (node->nodetype != LYS_LIST) {
                LOGVAL(ctx, LYE_NORESOLV, LY_VLOG_LYS, parent, "leafref", type->info.lref.path);
                ly_set_free(node_set);
                return -1;
            }

            i = resolve_schema_leafref_predicate(id, node, parent, node_set);
            if (!i) {
                ly_set_free(node_set);
                return EXIT_FAILURE;
            } else if (i < 0) {
                ly_set_free(node_set);
                return -1;
            }
            id += i;
            has_predicate = 0;
        }
    } while (id[0]);

    /* the target must be leaf or leaf-list (in YANG 1.1 only) */
    if ((node->nodetype != LYS_LEAF) && (node->nodetype != LYS_LEAFLIST)) {
        LOGVAL(ctx, LYE_NORESOLV, LY_VLOG_LYS, parent, "leafref", type->info.lref.path);
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_PREV, NULL, "Leafref target \"%s\" is not a leaf nor a leaf-list.", type->info.lref.path);
        ly_set_free(node_set);
        return -1;
    }

    /* check status */
    if (lyp_check_status(parent->flags, parent->module, parent->name,
                    node->flags, node->module, node->name, node)) {
        ly_set_free(node_set);
        return -1;
    }

    /* assign */
    type->info.lref.target = (struct lys_node_leaf *)node;

    /* add the target node into a set so its parent chain modules can be implemented */
    ly_set_add(node_set, (void *)node, 0);

    /* as the last thing traverse this leafref and make targets on the path implemented */
    if (lys_node_module(parent)->implemented) {
        /* make all the modules in the path implemented */
        for (i = 0; (unsigned)i < node_set->number; ++i) {
            for (node = node_set->set.s[i]; node; node = lys_parent(node)) {
                if (!lys_node_module(node)->implemented) {
                    lys_node_module(node)->implemented = 1;
                    if (unres_schema_add_node(lys_node_module(node), unres, NULL, UNRES_MOD_IMPLEMENT, NULL) == -1) {
                        ly_set_free(node_set);
                        return -1;
                    }
                }
            }
        }

        /* store the backlink from leafref target */
        if (lys_leaf_add_leafref_target(type->info.lref.target, (struct lys_node *)type->parent)) {
            ly_set_free(node_set);
            return -1;
        }
    }
    ly_set_free(node_set);

    /* check if leafref and its target are under common if-features */
    return check_leafref_features(type);
}