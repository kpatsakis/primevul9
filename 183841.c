yang_check_choice(struct lys_module *module, struct lys_node_choice *choice, struct lys_node **child,
                  int options, struct unres_schema *unres)
{
    char *value;

    if (yang_check_iffeatures(module, NULL, choice, CHOICE_KEYWORD, unres)) {
        free(choice->dflt);
        choice->dflt = NULL;
        goto error;
    }

    if (yang_check_nodes(module, (struct lys_node *)choice, *child, options, unres)) {
        *child = NULL;
        free(choice->dflt);
        choice->dflt = NULL;
        goto error;
    }
    *child = NULL;

    if (choice->dflt) {
        value = (char *)choice->dflt;
        choice->dflt = NULL;
        if (unres_schema_add_str(module, unres, choice, UNRES_CHOICE_DFLT, value) == -1) {
            free(value);
            goto error;
        }
        free(value);
    }

    if (choice->when && yang_check_ext_instance(module, &choice->when->ext, choice->when->ext_size, choice->when, unres)) {
        goto error;
    }

    /* check XPath dependencies */
    if (!(module->ctx->models.flags & LY_CTX_TRUSTED) && choice->when) {
        if (options & LYS_PARSE_OPT_INGRP) {
            if (lyxp_node_check_syntax((struct lys_node *)choice)) {
                goto error;
            }
        } else {
            if (unres_schema_add_node(module, unres, choice, UNRES_XPATH, NULL) == -1) {
                goto error;
            }
        }
    }

    return EXIT_SUCCESS;

error:
    return EXIT_FAILURE;
}