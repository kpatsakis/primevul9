yang_check_imports(struct lys_module *module, struct unres_schema *unres)
{
    struct lys_import *imp;
    struct lys_include *inc;
    uint8_t imp_size, inc_size, j = 0, i = 0;
    char *s;

    imp = module->imp;
    imp_size = module->imp_size;
    inc = module->inc;
    inc_size = module->inc_size;

    if (imp_size) {
        module->imp = calloc(imp_size, sizeof *module->imp);
        module->imp_size = 0;
        LY_CHECK_ERR_GOTO(!module->imp, LOGMEM(module->ctx), error);
    }

    if (inc_size) {
        module->inc = calloc(inc_size, sizeof *module->inc);
        module->inc_size = 0;
        LY_CHECK_ERR_GOTO(!module->inc, LOGMEM(module->ctx), error);
    }

    for (i = 0; i < imp_size; ++i) {
        s = (char *) imp[i].module;
        imp[i].module = NULL;
        if (yang_fill_import(module, &imp[i], &module->imp[module->imp_size], s, unres)) {
            ++i;
            goto error;
        }
    }
    for (j = 0; j < inc_size; ++j) {
        s = (char *) inc[j].submodule;
        inc[j].submodule = NULL;
        if (yang_fill_include(module, s, &inc[j], unres)) {
            ++j;
            goto error;
        }
    }
    free(inc);
    free(imp);

    return EXIT_SUCCESS;

error:
    yang_free_import(module->ctx, imp, i, imp_size);
    yang_free_include(module->ctx, inc, j, inc_size);
    free(imp);
    free(inc);
    return EXIT_FAILURE;
}