yang_check_notif(struct lys_module *module, struct lys_node_notif *notif, struct lys_node **child,
                 int options, struct unres_schema *unres)
{
    if (yang_check_typedef(module, (struct lys_node *)notif, unres)) {
        goto error;
    }

    if (yang_check_iffeatures(module, NULL, notif, NOTIFICATION_KEYWORD, unres)) {
        goto error;
    }

    if (yang_check_nodes(module, (struct lys_node *)notif, *child, options | LYS_PARSE_OPT_CFG_IGNORE, unres)) {
        *child = NULL;
        goto error;
    }
    *child = NULL;

    if (yang_check_must(module, notif->must, notif->must_size, unres)) {
        goto error;
    }

    /* check XPath dependencies */
    if (!(module->ctx->models.flags & LY_CTX_TRUSTED) && notif->must_size) {
        if (options & LYS_PARSE_OPT_INGRP) {
            if (lyxp_node_check_syntax((struct lys_node *)notif)) {
                goto error;
            }
        } else {
            if (unres_schema_add_node(module, unres, notif, UNRES_XPATH, NULL) == -1) {
                goto error;
            }
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}