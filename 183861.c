yang_check_leaf(struct lys_module *module, struct lys_node_leaf *leaf, int options, struct unres_schema *unres)
{
    if (yang_fill_type(module, &leaf->type, (struct yang_type *)leaf->type.der, leaf, unres)) {
        yang_type_free(module->ctx, &leaf->type);
        goto error;
    }
    if (yang_check_iffeatures(module, NULL, leaf, LEAF_KEYWORD, unres)) {
        yang_type_free(module->ctx, &leaf->type);
        goto error;
    }

    if (unres_schema_add_node(module, unres, &leaf->type, UNRES_TYPE_DER, (struct lys_node *)leaf) == -1) {
        yang_type_free(module->ctx, &leaf->type);
        goto error;
    }

    if (!(module->ctx->models.flags & LY_CTX_TRUSTED) &&
            (unres_schema_add_node(module, unres, &leaf->type, UNRES_TYPE_DFLT, (struct lys_node *)&leaf->dflt) == -1)) {
        goto error;
    }

    if (leaf->when && yang_check_ext_instance(module, &leaf->when->ext, leaf->when->ext_size, leaf->when, unres)) {
        goto error;
    }
    if (yang_check_must(module, leaf->must, leaf->must_size, unres)) {
        goto error;
    }

    /* check XPath dependencies */
    if (!(module->ctx->models.flags & LY_CTX_TRUSTED) && (leaf->when || leaf->must_size)) {
        if (options & LYS_PARSE_OPT_INGRP) {
            if (lyxp_node_check_syntax((struct lys_node *)leaf)) {
                goto error;
            }
        } else {
            if (unres_schema_add_node(module, unres, leaf, UNRES_XPATH, NULL) == -1) {
                goto error;
            }
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}