yang_check_sub_module(struct lys_module *module, struct unres_schema *unres, struct lys_node *node)
{
    uint i, erase_identities = 1, erase_nodes = 1, aug_size, dev_size = 0;

    aug_size = module->augment_size;
    module->augment_size = 0;
    dev_size = module->deviation_size;
    module->deviation_size = 0;

    if (yang_check_typedef(module, NULL, unres)) {
        goto error;
    }

    if (yang_check_ext_instance(module, &module->ext, module->ext_size, module, unres)) {
        goto error;
    }

    /* check extension in revision */
    for (i = 0; i < module->rev_size; ++i) {
        if (yang_check_ext_instance(module, &module->rev[i].ext, module->rev[i].ext_size, &module->rev[i], unres)) {
            goto error;
        }
    }

    /* check extension in definition of extension */
    for (i = 0; i < module->extensions_size; ++i) {
        if (yang_check_ext_instance(module, &module->extensions[i].ext, module->extensions[i].ext_size, &module->extensions[i], unres)) {
            goto error;
        }
    }

    /* check features */
    for (i = 0; i < module->features_size; ++i) {
        if (yang_check_iffeatures(module, NULL, &module->features[i], FEATURE_KEYWORD, unres)) {
            goto error;
        }
        if (yang_check_ext_instance(module, &module->features[i].ext, module->features[i].ext_size, &module->features[i], unres)) {
            goto error;
        }

        /* check for circular dependencies */
        if (module->features[i].iffeature_size && (unres_schema_add_node(module, unres, &module->features[i], UNRES_FEATURE, NULL) == -1)) {
            goto error;
        }
    }
    erase_identities = 0;
    if (yang_check_identities(module, unres)) {
        goto error;
    }
    erase_nodes = 0;
    if (yang_check_nodes(module, NULL, node, 0, unres)) {
        goto error;
    }

    /* check deviation */
    for (i = 0; i < dev_size; ++i) {
        module->deviation_size++;
        if (yang_check_deviation(module, unres, &module->deviation[i])) {
            goto error;
        }
    }

    /* check augments */
    for (i = 0; i < aug_size; ++i) {
        module->augment_size++;
        if (yang_check_augment(module, &module->augment[i], 0, unres)) {
            goto error;
        }
        if (unres_schema_add_node(module, unres, &module->augment[i], UNRES_AUGMENT, NULL) == -1) {
            goto error;
        }
    }

    return EXIT_SUCCESS;

error:
    if (erase_identities) {
        yang_free_ident_base(module->ident, 0, module->ident_size);
    }
    if (erase_nodes) {
        yang_free_nodes(module->ctx, node);
    }
    for (i = module->augment_size; i < aug_size; ++i) {
        yang_free_augment(module->ctx, &module->augment[i]);
    }
    for (i = module->deviation_size; i < dev_size; ++i) {
        yang_free_deviate(module->ctx, &module->deviation[i], 0);
        free(module->deviation[i].deviate);
        lydict_remove(module->ctx, module->deviation[i].target_name);
        lydict_remove(module->ctx, module->deviation[i].dsc);
        lydict_remove(module->ctx, module->deviation[i].ref);
    }
    return EXIT_FAILURE;
}