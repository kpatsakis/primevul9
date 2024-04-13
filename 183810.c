yang_check_list(struct lys_module *module, struct lys_node_list *list, struct lys_node **child,
                int options, struct unres_schema *unres)
{
    struct lys_node *node;

    if (yang_check_typedef(module, (struct lys_node *)list, unres)) {
        goto error;
    }

    if (yang_check_iffeatures(module, NULL, list, LIST_KEYWORD, unres)) {
        goto error;
    }

    if (list->flags & LYS_CONFIG_R) {
        /* RFC 6020, 7.7.5 - ignore ordering when the list represents state data
         * ignore oredering MASK - 0x7F
         */
        list->flags &= 0x7F;
    }
    /* check - if list is configuration, key statement is mandatory
     * (but only if we are not in a grouping or augment, then the check is deferred) */
    for (node = (struct lys_node *)list; node && !(node->nodetype & (LYS_GROUPING | LYS_AUGMENT | LYS_EXT)); node = node->parent);
    if (!node && (list->flags & LYS_CONFIG_W) && !list->keys) {
        LOGVAL(module->ctx, LYE_MISSCHILDSTMT, LY_VLOG_LYS, list, "key", "list");
        goto error;
    }

    if (yang_check_nodes(module, (struct lys_node *)list, *child, options, unres)) {
        *child = NULL;
        goto error;
    }
    *child = NULL;

    if (list->keys && yang_read_key(module, list, unres)) {
        goto error;
    }

    if (yang_read_unique(module, list, unres)) {
        goto error;
    }

    if (list->when && yang_check_ext_instance(module, &list->when->ext, list->when->ext_size, list->when, unres)) {
        goto error;
    }
    if (yang_check_must(module, list->must, list->must_size, unres)) {
        goto error;
    }

    /* check XPath dependencies */
    if (!(module->ctx->models.flags & LY_CTX_TRUSTED) && (list->when || list->must_size)) {
        if (options & LYS_PARSE_OPT_INGRP) {
            if (lyxp_node_check_syntax((struct lys_node *)list)) {
                goto error;
            }
        } else {
            if (unres_schema_add_node(module, unres, list, UNRES_XPATH, NULL) == -1) {
                goto error;
            }
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}