yang_check_augment(struct lys_module *module, struct lys_node_augment *augment, int options, struct unres_schema *unres)
{
    struct lys_node *child;

    child = augment->child;
    augment->child = NULL;

    if (yang_check_iffeatures(module, NULL, augment, AUGMENT_KEYWORD, unres)) {
        yang_free_nodes(module->ctx, child);
        goto error;
    }

    if (yang_check_nodes(module, (struct lys_node *)augment, child, options, unres)) {
        goto error;
    }

    if (yang_check_ext_instance(module, &augment->ext, augment->ext_size, augment, unres)) {
        goto error;
    }

    if (augment->when && yang_check_ext_instance(module, &augment->when->ext, augment->when->ext_size, augment->when, unres)) {
        goto error;
    }

    /* check XPath dependencies */
    if (!(module->ctx->models.flags & LY_CTX_TRUSTED) && augment->when) {
        if (options & LYS_PARSE_OPT_INGRP) {
            if (lyxp_node_check_syntax((struct lys_node *)augment)) {
                goto error;
            }
        } else {
            if (unres_schema_add_node(module, unres, augment, UNRES_XPATH, NULL) == -1) {
                goto error;
            }
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}