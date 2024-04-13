yang_check_container(struct lys_module *module, struct lys_node_container *cont, struct lys_node **child,
                     int options, struct unres_schema *unres)
{
    if (yang_check_typedef(module, (struct lys_node *)cont, unres)) {
        goto error;
    }

    if (yang_check_iffeatures(module, NULL, cont, CONTAINER_KEYWORD, unres)) {
        goto error;
    }

    if (yang_check_nodes(module, (struct lys_node *)cont, *child, options, unres)) {
        *child = NULL;
        goto error;
    }
    *child = NULL;

    if (cont->when && yang_check_ext_instance(module, &cont->when->ext, cont->when->ext_size, cont->when, unres)) {
        goto error;
    }
    if (yang_check_must(module, cont->must, cont->must_size, unres)) {
        goto error;
    }

    /* check XPath dependencies */
    if (!(module->ctx->models.flags & LY_CTX_TRUSTED) && (cont->when || cont->must_size)) {
        if (options & LYS_PARSE_OPT_INGRP) {
            if (lyxp_node_check_syntax((struct lys_node *)cont)) {
                goto error;
            }
        } else {
            if (unres_schema_add_node(module, unres, cont, UNRES_XPATH, NULL) == -1) {
                goto error;
            }
        }
    }

    return EXIT_SUCCESS;
error:

    return EXIT_FAILURE;
}