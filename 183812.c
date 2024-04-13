yang_check_uses(struct lys_module *module, struct lys_node_uses *uses, int options, struct unres_schema *unres)
{
    uint i, size;

    size = uses->augment_size;
    uses->augment_size = 0;

    if (yang_check_iffeatures(module, NULL, uses, USES_KEYWORD, unres)) {
        goto error;
    }

    for (i = 0; i < uses->refine_size; ++i) {
        if (yang_check_iffeatures(module, &uses->refine[i], uses, REFINE_KEYWORD, unres)) {
            goto error;
        }
        if (yang_check_must(module, uses->refine[i].must, uses->refine[i].must_size, unres)) {
            goto error;
        }
        if (yang_check_ext_instance(module, &uses->refine[i].ext, uses->refine[i].ext_size, &uses->refine[i], unres)) {
            goto error;
        }
    }

    for (i = 0; i < size; ++i) {
        uses->augment_size++;
        if (yang_check_augment(module, &uses->augment[i], options, unres)) {
            goto error;
        }
    }

    if (unres_schema_add_node(module, unres, uses, UNRES_USES, NULL) == -1) {
        goto error;
    }

    if (uses->when && yang_check_ext_instance(module, &uses->when->ext, uses->when->ext_size, uses->when, unres)) {
        goto error;
    }

    /* check XPath dependencies */
    if (!(module->ctx->models.flags & LY_CTX_TRUSTED) && uses->when) {
        if (options & LYS_PARSE_OPT_INGRP) {
            if (lyxp_node_check_syntax((struct lys_node *)uses)) {
                goto error;
            }
        } else {
            if (unres_schema_add_node(module, unres, uses, UNRES_XPATH, NULL) == -1) {
                goto error;
            }
        }
    }

    return EXIT_SUCCESS;

error:
    for (i = uses->augment_size; i < size; ++i) {
        yang_free_augment(module->ctx, &uses->augment[i]);
    }
    return EXIT_FAILURE;
}