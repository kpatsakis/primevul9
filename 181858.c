lyp_check_include(struct lys_module *module, const char *value, struct lys_include *inc, struct unres_schema *unres)
{
    int i;

    /* check that the submodule was not included yet */
    i = lyp_check_includedup(module, value, inc, &inc->submodule);
    if (i == -1) {
        return -1;
    } else if (i == 1) {
        return 0;
    }
    /* submodule is not yet loaded */

    /* circular include check */
    if (lyp_check_circmod(module, value, 0)) {
        return -1;
    }

    /* try to load the submodule */
    inc->submodule = (struct lys_submodule *)ly_ctx_load_sub_module(module->ctx, module, value,
                                                                    inc->rev[0] ? inc->rev : NULL, 1, unres);

    /* check the result */
    if (!inc->submodule) {
        if (ly_errno != LY_EVALID) {
            LOGVAL(module->ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "include");
        }
        LOGERR(module->ctx, LY_EVALID, "Including \"%s\" module into \"%s\" failed.", value, module->name);
        return -1;
    }

    /* check the revision */
    if (inc->rev[0] && inc->submodule->rev_size && strcmp(inc->rev, inc->submodule->rev[0].date)) {
        LOGERR(module->ctx, LY_EVALID, "\"%s\" include of submodule \"%s\" in revision \"%s\" not found.",
               module->name, value, inc->rev);
        unres_schema_free((struct lys_module *)inc->submodule, &unres, 0);
        lys_sub_module_remove_devs_augs((struct lys_module *)inc->submodule);
        lys_submodule_module_data_free((struct lys_submodule *)inc->submodule);
        lys_submodule_free(inc->submodule, NULL);
        inc->submodule = NULL;
        return -1;
    }

    /* store the submodule as successfully parsed */
    lyp_add_includedup(module, inc->submodule);

    return 0;
}