resolve_union(struct lyd_node_leaf_list *leaf, struct lys_type *type, int store, int ignore_fail,
              struct lys_type **resolved_type)
{
    struct ly_ctx *ctx = leaf->schema->module->ctx;
    struct lys_type *t;
    struct lyd_node *ret;
    enum int_log_opts prev_ilo;
    int found, success = 0, ext_dep, req_inst;
    const char *json_val = NULL;

    assert(type->base == LY_TYPE_UNION);

    if ((leaf->value_type == LY_TYPE_UNION) || ((leaf->value_type == LY_TYPE_INST) && (leaf->value_flags & LY_VALUE_UNRES))) {
        /* either NULL or instid previously converted to JSON */
        json_val = lydict_insert(ctx, leaf->value.string, 0);
    }

    if (store) {
        lyd_free_value(leaf->value, leaf->value_type, leaf->value_flags, &((struct lys_node_leaf *)leaf->schema)->type,
                       leaf->value_str, NULL, NULL, NULL);
        memset(&leaf->value, 0, sizeof leaf->value);
    }

    /* turn logging off, we are going to try to validate the value with all the types in order */
    ly_ilo_change(NULL, ILO_IGNORE, &prev_ilo, 0);

    t = NULL;
    found = 0;
    while ((t = lyp_get_next_union_type(type, t, &found))) {
        found = 0;

        switch (t->base) {
        case LY_TYPE_LEAFREF:
            if ((ignore_fail == 1) || ((leaf->schema->flags & LYS_LEAFREF_DEP) && (ignore_fail == 2))) {
                req_inst = -1;
            } else {
                req_inst = t->info.lref.req;
            }

            if (!resolve_leafref(leaf, t->info.lref.path, req_inst, &ret)) {
                if (store) {
                    if (ret && !(leaf->schema->flags & LYS_LEAFREF_DEP)) {
                        /* valid resolved */
                        leaf->value.leafref = ret;
                        leaf->value_type = LY_TYPE_LEAFREF;
                    } else {
                        /* valid unresolved */
                        ly_ilo_restore(NULL, prev_ilo, NULL, 0);
                        if (!lyp_parse_value(t, &leaf->value_str, NULL, leaf, NULL, NULL, 1, 0, 0)) {
                            return -1;
                        }
                        ly_ilo_change(NULL, ILO_IGNORE, &prev_ilo, NULL);
                    }
                }

                success = 1;
            }
            break;
        case LY_TYPE_INST:
            ext_dep = check_instid_ext_dep(leaf->schema, (json_val ? json_val : leaf->value_str));
            if ((ignore_fail == 1) || (ext_dep && (ignore_fail == 2))) {
                req_inst = -1;
            } else {
                req_inst = t->info.inst.req;
            }

            if (!resolve_instid((struct lyd_node *)leaf, (json_val ? json_val : leaf->value_str), req_inst, &ret)) {
                if (store) {
                    if (ret && !ext_dep) {
                        /* valid resolved */
                        leaf->value.instance = ret;
                        leaf->value_type = LY_TYPE_INST;

                        if (json_val) {
                            lydict_remove(leaf->schema->module->ctx, leaf->value_str);
                            leaf->value_str = json_val;
                            json_val = NULL;
                        }
                    } else {
                        /* valid unresolved */
                        if (json_val) {
                            /* put the JSON val back */
                            leaf->value.string = json_val;
                            json_val = NULL;
                        } else {
                            leaf->value.instance = NULL;
                        }
                        leaf->value_type = LY_TYPE_INST;
                        leaf->value_flags |= LY_VALUE_UNRES;
                    }
                }

                success = 1;
            }
            break;
        default:
            if (lyp_parse_value(t, &leaf->value_str, NULL, leaf, NULL, NULL, store, 0, 0)) {
                success = 1;
            }
            break;
        }

        if (success) {
            break;
        }

        /* erase possible present and invalid value data */
        if (store) {
            lyd_free_value(leaf->value, leaf->value_type, leaf->value_flags, t, leaf->value_str, NULL, NULL, NULL);
            memset(&leaf->value, 0, sizeof leaf->value);
        }
    }

    /* turn logging back on */
    ly_ilo_restore(NULL, prev_ilo, NULL, 0);

    if (json_val) {
        if (!success) {
            /* put the value back for now */
            assert(leaf->value_type == LY_TYPE_UNION);
            leaf->value.string = json_val;
        } else {
            /* value was ultimately useless, but we could not have known */
            lydict_remove(leaf->schema->module->ctx, json_val);
        }
    }

    if (success) {
        if (resolved_type) {
            *resolved_type = t;
        }
    } else if (!ignore_fail || !type->info.uni.has_ptr_type) {
        /* not found and it is required */
        LOGVAL(ctx, LYE_INVAL, LY_VLOG_LYD, leaf, leaf->value_str ? leaf->value_str : "", leaf->schema->name);
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;

}