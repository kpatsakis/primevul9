yang_read_submodule(struct lys_module *module, const char *data, unsigned int size, struct unres_schema *unres)
{
    struct lys_submodule *submodule;
    struct lys_node *node = NULL;

    submodule = calloc(1, sizeof *submodule);
    LY_CHECK_ERR_GOTO(!submodule, LOGMEM(module->ctx), error);

    submodule->ctx = module->ctx;
    submodule->type = 1;
    submodule->implemented = module->implemented;
    submodule->belongsto = module;

    /* add into the list of processed modules */
    if (lyp_check_circmod_add((struct lys_module *)submodule)) {
        goto error;
    }

    /* module cannot be changed in this case and 1 cannot be returned */
    if (yang_parse_mem(module, submodule, unres, data, size, &node)) {
        free_yang_common((struct lys_module *)submodule, node);
        goto error;
    }

    lyp_sort_revisions((struct lys_module *)submodule);

    if (yang_check_sub_module((struct lys_module *)submodule, unres, node)) {
        goto error;
    }

    lyp_check_circmod_pop(module->ctx);

    LOGVRB("Submodule \"%s\" successfully parsed.", submodule->name);
    return submodule;

error:
    /* cleanup */
    if (!submodule || !submodule->name) {
        free(submodule);
        LOGERR(module->ctx, ly_errno, "Submodule parsing failed.");
        return NULL;
    }

    LOGERR(module->ctx, ly_errno, "Submodule \"%s\" parsing failed.", submodule->name);

    unres_schema_free((struct lys_module *)submodule, &unres, 0);
    lyp_check_circmod_pop(module->ctx);
    lys_sub_module_remove_devs_augs((struct lys_module *)submodule);
    lys_submodule_module_data_free(submodule);
    lys_submodule_free(submodule, NULL);
    return NULL;
}