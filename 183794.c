yang_check_anydata(struct lys_module *module, struct lys_node_anydata *anydata, struct lys_node **child,
                   int options, struct unres_schema *unres)
{
    if (yang_check_iffeatures(module, NULL, anydata, ANYDATA_KEYWORD, unres)) {
        goto error;
    }

    if (yang_check_nodes(module, (struct lys_node *)anydata, *child, options, unres)) {
        *child = NULL;
        goto error;
    }
    *child = NULL;

    if (anydata->when && yang_check_ext_instance(module, &anydata->when->ext, anydata->when->ext_size, anydata->when, unres)) {
        goto error;
    }
    if (yang_check_must(module, anydata->must, anydata->must_size, unres)) {
        goto error;
    }

    /* check XPath dependencies */
    if (!(module->ctx->models.flags & LY_CTX_TRUSTED) && (anydata->when || anydata->must_size)) {
        if (options & LYS_PARSE_OPT_INGRP) {
            if (lyxp_node_check_syntax((struct lys_node *)anydata)) {
                goto error;
            }
        } else {
            if (unres_schema_add_node(module, unres, anydata, UNRES_XPATH, NULL) == -1) {
                goto error;
            }
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}