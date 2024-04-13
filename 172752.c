check_default(struct lys_type *type, const char **value, struct lys_module *module, int tpdf)
{
    struct lys_tpdf *base_tpdf = NULL;
    struct lyd_node_leaf_list node;
    const char *dflt = NULL;
    char *s;
    int ret = EXIT_SUCCESS, r;
    struct ly_ctx *ctx = module->ctx;

    assert(value);
    memset(&node, 0, sizeof node);

    if (type->base <= LY_TYPE_DER) {
        /* the type was not resolved yet, nothing to do for now */
        ret = EXIT_FAILURE;
        goto cleanup;
    } else if (!tpdf && !module->implemented) {
        /* do not check defaults in not implemented module's data */
        goto cleanup;
    } else if (tpdf && !module->implemented && type->base == LY_TYPE_IDENT) {
        /* identityrefs are checked when instantiated in data instead of typedef,
         * but in typedef the value has to be modified to include the prefix */
        if (*value) {
            if (strchr(*value, ':')) {
                dflt = transform_schema2json(module, *value);
            } else {
                /* default prefix of the module where the typedef is defined */
                if (asprintf(&s, "%s:%s", lys_main_module(module)->name, *value) == -1) {
                    LOGMEM(ctx);
                    ret = -1;
                    goto cleanup;
                }
                dflt = lydict_insert_zc(ctx, s);
            }
            lydict_remove(ctx, *value);
            *value = dflt;
            dflt = NULL;
        }
        goto cleanup;
    } else if (type->base == LY_TYPE_LEAFREF && tpdf) {
        /* leafref in typedef cannot be checked */
        goto cleanup;
    }

    dflt = lydict_insert(ctx, *value, 0);
    if (!dflt) {
        /* we do not have a new default value, so is there any to check even, in some base type? */
        for (base_tpdf = type->der; base_tpdf->type.der; base_tpdf = base_tpdf->type.der) {
            if (base_tpdf->dflt) {
                dflt = lydict_insert(ctx, base_tpdf->dflt, 0);
                break;
            }
        }

        if (!dflt) {
            /* no default value, nothing to check, all is well */
            goto cleanup;
        }

        /* so there is a default value in a base type, but can the default value be no longer valid (did we define some new restrictions)? */
        switch (type->base) {
        case LY_TYPE_IDENT:
            if (lys_main_module(base_tpdf->type.parent->module)->implemented) {
                goto cleanup;
            } else {
                /* check the default value from typedef, but use also the typedef's module
                 * due to possible searching in imported modules which is expected in
                 * typedef's module instead of module where the typedef is used */
                module = base_tpdf->module;
            }
            break;
        case LY_TYPE_INST:
        case LY_TYPE_LEAFREF:
        case LY_TYPE_BOOL:
        case LY_TYPE_EMPTY:
            /* these have no restrictions, so we would do the exact same work as the unres in the base typedef */
            goto cleanup;
        case LY_TYPE_BITS:
            /* the default value must match the restricted list of values, if the type was restricted */
            if (type->info.bits.count) {
                break;
            }
            goto cleanup;
        case LY_TYPE_ENUM:
            /* the default value must match the restricted list of values, if the type was restricted */
            if (type->info.enums.count) {
                break;
            }
            goto cleanup;
        case LY_TYPE_DEC64:
            if (type->info.dec64.range) {
                break;
            }
            goto cleanup;
        case LY_TYPE_BINARY:
            if (type->info.binary.length) {
                break;
            }
            goto cleanup;
        case LY_TYPE_INT8:
        case LY_TYPE_INT16:
        case LY_TYPE_INT32:
        case LY_TYPE_INT64:
        case LY_TYPE_UINT8:
        case LY_TYPE_UINT16:
        case LY_TYPE_UINT32:
        case LY_TYPE_UINT64:
            if (type->info.num.range) {
                break;
            }
            goto cleanup;
        case LY_TYPE_STRING:
            if (type->info.str.length || type->info.str.patterns) {
                break;
            }
            goto cleanup;
        case LY_TYPE_UNION:
            /* way too much trouble learning whether we need to check the default again, so just do it */
            break;
        default:
            LOGINT(ctx);
            ret = -1;
            goto cleanup;
        }
    } else if (type->base == LY_TYPE_EMPTY) {
        LOGVAL(ctx, LYE_INCHILDSTMT, LY_VLOG_NONE, NULL, "default", type->parent->name);
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "The \"empty\" data type cannot have a default value.");
        ret = -1;
        goto cleanup;
    }

    /* dummy leaf */
    memset(&node, 0, sizeof node);
    node.value_str = lydict_insert(ctx, dflt, 0);
    node.value_type = type->base;

    if (tpdf) {
        node.schema = calloc(1, sizeof (struct lys_node_leaf));
        if (!node.schema) {
            LOGMEM(ctx);
            ret = -1;
            goto cleanup;
        }
        r = asprintf((char **)&node.schema->name, "typedef-%s-default", ((struct lys_tpdf *)type->parent)->name);
        if (r == -1) {
            LOGMEM(ctx);
            ret = -1;
            goto cleanup;
        }
        node.schema->module = module;
        memcpy(&((struct lys_node_leaf *)node.schema)->type, type, sizeof *type);
    } else {
        node.schema = (struct lys_node *)type->parent;
    }

    if (type->base == LY_TYPE_LEAFREF) {
        if (!type->info.lref.target) {
            ret = EXIT_FAILURE;
            LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "Default value \"%s\" cannot be checked in an unresolved leafref.",
                   dflt);
            goto cleanup;
        }
        ret = check_default(&type->info.lref.target->type, &dflt, module, 0);
        if (!ret) {
            /* adopt possibly changed default value to its canonical form */
            if (*value) {
                lydict_remove(ctx, *value);
                *value = dflt;
                dflt = NULL;
            }
        }
    } else {
        if (!lyp_parse_value(type, &node.value_str, NULL, &node, NULL, module, 1, 1, 0)) {
            /* possible forward reference */
            ret = EXIT_FAILURE;
            if (base_tpdf) {
                /* default value is defined in some base typedef */
                if ((type->base == LY_TYPE_BITS && type->der->type.der) ||
                        (type->base == LY_TYPE_ENUM && type->der->type.der)) {
                    /* we have refined bits/enums */
                    LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL,
                           "Invalid value \"%s\" of the default statement inherited to \"%s\" from \"%s\" base type.",
                           dflt, type->parent->name, base_tpdf->name);
                }
            }
        } else {
            /* success - adopt canonical form from the node into the default value */
            if (!ly_strequal(dflt, node.value_str, 1)) {
                /* this can happen only if we have non-inherited default value,
                 * inherited default values are already in canonical form */
                assert(ly_strequal(dflt, *value, 1));

                lydict_remove(ctx, *value);
                *value = node.value_str;
                node.value_str = NULL;
            }
        }
    }

cleanup:
    lyd_free_value(node.value, node.value_type, node.value_flags, type, node.value_str, NULL, NULL, NULL);
    lydict_remove(ctx, node.value_str);
    if (tpdf && node.schema) {
        free((char *)node.schema->name);
        free(node.schema);
    }
    lydict_remove(ctx, dflt);

    return ret;
}