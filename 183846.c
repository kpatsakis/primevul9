yang_check_leaflist(struct lys_module *module, struct lys_node_leaflist *leaflist, int options,
                    struct unres_schema *unres)
{
    int i, j;

    if (yang_fill_type(module, &leaflist->type, (struct yang_type *)leaflist->type.der, leaflist, unres)) {
        yang_type_free(module->ctx, &leaflist->type);
        goto error;
    }
    if (yang_check_iffeatures(module, NULL, leaflist, LEAF_LIST_KEYWORD, unres)) {
        yang_type_free(module->ctx, &leaflist->type);
        goto error;
    }

    if (unres_schema_add_node(module, unres, &leaflist->type, UNRES_TYPE_DER, (struct lys_node *)leaflist) == -1) {
        yang_type_free(module->ctx, &leaflist->type);
        goto error;
    }

    for (i = 0; i < leaflist->dflt_size; ++i) {
        /* check for duplicity in case of configuration data,
         * in case of status data duplicities are allowed */
        if (leaflist->flags & LYS_CONFIG_W) {
            for (j = i +1; j < leaflist->dflt_size; ++j) {
                if (ly_strequal(leaflist->dflt[i], leaflist->dflt[j], 1)) {
                    LOGVAL(module->ctx, LYE_INARG, LY_VLOG_LYS, leaflist, leaflist->dflt[i], "default");
                    LOGVAL(module->ctx, LYE_SPEC, LY_VLOG_LYS, leaflist, "Duplicated default value \"%s\".", leaflist->dflt[i]);
                    goto error;
                }
            }
        }
        /* check default value (if not defined, there still could be some restrictions
         * that need to be checked against a default value from a derived type) */
        if (!(module->ctx->models.flags & LY_CTX_TRUSTED) &&
                (unres_schema_add_node(module, unres, &leaflist->type, UNRES_TYPE_DFLT,
                                       (struct lys_node *)(&leaflist->dflt[i])) == -1)) {
            goto error;
        }
    }

    if (leaflist->when && yang_check_ext_instance(module, &leaflist->when->ext, leaflist->when->ext_size, leaflist->when, unres)) {
        goto error;
    }
    if (yang_check_must(module, leaflist->must, leaflist->must_size, unres)) {
        goto error;
    }

    /* check XPath dependencies */
    if (!(module->ctx->models.flags & LY_CTX_TRUSTED) && (leaflist->when || leaflist->must_size)) {
        if (options & LYS_PARSE_OPT_INGRP) {
            if (lyxp_node_check_syntax((struct lys_node *)leaflist)) {
                goto error;
            }
        } else {
            if (unres_schema_add_node(module, unres, leaflist, UNRES_XPATH, NULL) == -1) {
                goto error;
            }
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}