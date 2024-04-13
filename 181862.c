lyp_add_includedup(struct lys_module *sub_mod, struct lys_submodule *parsed_submod)
{
    struct ly_modules_list *models = &sub_mod->ctx->models;
    int16_t i;

    /* store main module if first include */
    if (models->parsed_submodules_count) {
        for (i = models->parsed_submodules_count - 1; models->parsed_submodules[i]->type; --i);
    } else {
        i = -1;
    }
    if ((i == -1) || (models->parsed_submodules[i] != lys_main_module(sub_mod))) {
        ++models->parsed_submodules_count;
        models->parsed_submodules = ly_realloc(models->parsed_submodules,
                                               models->parsed_submodules_count * sizeof *models->parsed_submodules);
        LY_CHECK_ERR_RETURN(!models->parsed_submodules, LOGMEM(sub_mod->ctx), );
        models->parsed_submodules[models->parsed_submodules_count - 1] = lys_main_module(sub_mod);
    }

    /* store parsed submodule */
    ++models->parsed_submodules_count;
    models->parsed_submodules = ly_realloc(models->parsed_submodules,
                                           models->parsed_submodules_count * sizeof *models->parsed_submodules);
    LY_CHECK_ERR_RETURN(!models->parsed_submodules, LOGMEM(sub_mod->ctx), );
    models->parsed_submodules[models->parsed_submodules_count - 1] = (struct lys_module *)parsed_submod;
}