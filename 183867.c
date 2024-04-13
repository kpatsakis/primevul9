yang_read_module(struct ly_ctx *ctx, const char* data, unsigned int size, const char *revision, int implement)
{
    struct lys_module *module = NULL, *tmp_mod;
    struct unres_schema *unres = NULL;
    struct lys_node *node = NULL;
    int ret;

    unres = calloc(1, sizeof *unres);
    LY_CHECK_ERR_GOTO(!unres, LOGMEM(ctx), error);

    module = calloc(1, sizeof *module);
    LY_CHECK_ERR_GOTO(!module, LOGMEM(ctx), error);

    /* initiale module */
    module->ctx = ctx;
    module->type = 0;
    module->implemented = (implement ? 1 : 0);

    /* add into the list of processed modules */
    if (lyp_check_circmod_add(module)) {
        goto error;
    }

    ret = yang_parse_mem(module, NULL, unres, data, size, &node);
    if (ret == -1) {
        if (ly_vecode(ctx) == LYVE_SUBMODULE && !module->name) {
            /* Remove this module from the list of processed modules,
               as we're about to free it */
            lyp_check_circmod_pop(ctx);

            free(module);
            module = NULL;
        } else {
            free_yang_common(module, node);
        }
        goto error;
    } else if (ret == 1) {
        assert(!unres->count);
    } else {
        if (yang_check_sub_module(module, unres, node)) {
            goto error;
        }

        if (!implement && module->implemented && lys_make_implemented_r(module, unres)) {
            goto error;
        }

        if (unres->count && resolve_unres_schema(module, unres)) {
            goto error;
        }

        /* check correctness of includes */
        if (lyp_check_include_missing(module)) {
            goto error;
        }
    }

    lyp_sort_revisions(module);

    if (lyp_rfn_apply_ext(module) || lyp_deviation_apply_ext(module)) {
        goto error;
    }

    if (revision) {
        /* check revision of the parsed model */
        if (!module->rev_size || strcmp(revision, module->rev[0].date)) {
            LOGVRB("Module \"%s\" parsed with the wrong revision (\"%s\" instead \"%s\").",
                   module->name, module->rev[0].date, revision);
            goto error;
        }
    }

    /* add into context if not already there */
    if (!ret) {
        if (lyp_ctx_add_module(module)) {
            goto error;
        }

        /* remove our submodules from the parsed submodules list */
        lyp_del_includedup(module, 0);
    } else {
        tmp_mod = module;

        /* get the model from the context */
        module = (struct lys_module *)ly_ctx_get_module(ctx, module->name, revision, 0);
        assert(module);

        /* free what was parsed */
        lys_free(tmp_mod, NULL, 0, 0);
    }

    unres_schema_free(NULL, &unres, 0);
    lyp_check_circmod_pop(ctx);
    LOGVRB("Module \"%s%s%s\" successfully parsed as %s.", module->name, (module->rev_size ? "@" : ""),
           (module->rev_size ? module->rev[0].date : ""), (module->implemented ? "implemented" : "imported"));
    return module;

error:
    /* cleanup */
    unres_schema_free(module, &unres, 1);

    if (!module) {
        if (ly_vecode(ctx) != LYVE_SUBMODULE) {
            LOGERR(ctx, ly_errno, "Module parsing failed.");
        }
        return NULL;
    }

    if (module->name) {
        LOGERR(ctx, ly_errno, "Module \"%s\" parsing failed.", module->name);
    } else {
        LOGERR(ctx, ly_errno, "Module parsing failed.");
    }

    lyp_check_circmod_pop(ctx);
    lys_sub_module_remove_devs_augs(module);
    lyp_del_includedup(module, 1);
    lys_free(module, NULL, 0, 1);
    return NULL;
}