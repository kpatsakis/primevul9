lyp_get_module(const struct lys_module *module, const char *prefix, int pref_len, const char *name, int name_len, int in_data)
{
    const struct lys_module *main_module;
    char *str;
    int i;

    assert(!prefix || !name);

    if (prefix && !pref_len) {
        pref_len = strlen(prefix);
    }
    if (name && !name_len) {
        name_len = strlen(name);
    }

    main_module = lys_main_module(module);

    /* module own prefix, submodule own prefix, (sub)module own name */
    if ((!prefix || (!module->type && !strncmp(main_module->prefix, prefix, pref_len) && !main_module->prefix[pref_len])
                 || (module->type && !strncmp(module->prefix, prefix, pref_len) && !module->prefix[pref_len]))
            && (!name || (!strncmp(main_module->name, name, name_len) && !main_module->name[name_len]))) {
        return main_module;
    }

    /* standard import */
    for (i = 0; i < module->imp_size; ++i) {
        if ((!prefix || (!strncmp(module->imp[i].prefix, prefix, pref_len) && !module->imp[i].prefix[pref_len]))
                && (!name || (!strncmp(module->imp[i].module->name, name, name_len) && !module->imp[i].module->name[name_len]))) {
            return module->imp[i].module;
        }
    }

    /* module required by a foreign grouping, deviation, or submodule */
    if (name) {
        str = strndup(name, name_len);
        if (!str) {
            LOGMEM(module->ctx);
            return NULL;
        }
        main_module = ly_ctx_get_module(module->ctx, str, NULL, 0);

        /* try data callback */
        if (!main_module && in_data && module->ctx->data_clb) {
            main_module = module->ctx->data_clb(module->ctx, str, NULL, 0, module->ctx->data_clb_data);
        }

        free(str);
        return main_module;
    }

    return NULL;
}