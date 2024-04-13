lyp_ctx_add_module(struct lys_module *module)
{
    struct lys_module **newlist = NULL;
    int i;

    assert(!lyp_ctx_check_module(module));

#ifndef NDEBUG
    int j;
    /* check that all augments are resolved */
    for (i = 0; i < module->augment_size; ++i) {
        assert(module->augment[i].target);
    }
    for (i = 0; i < module->inc_size; ++i) {
        for (j = 0; j < module->inc[i].submodule->augment_size; ++j) {
            assert(module->inc[i].submodule->augment[j].target);
        }
    }
#endif

    /* add to the context's list of modules */
    if (module->ctx->models.used == module->ctx->models.size) {
        newlist = realloc(module->ctx->models.list, (2 * module->ctx->models.size) * sizeof *newlist);
        LY_CHECK_ERR_RETURN(!newlist, LOGMEM(module->ctx), -1);
        for (i = module->ctx->models.size; i < module->ctx->models.size * 2; i++) {
            newlist[i] = NULL;
        }
        module->ctx->models.size *= 2;
        module->ctx->models.list = newlist;
    }
    module->ctx->models.list[module->ctx->models.used++] = module;
    module->ctx->models.module_set_id++;

    return 0;
}