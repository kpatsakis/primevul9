lyp_check_circmod_add(struct lys_module *module)
{
    struct ly_modules_list *models = &module->ctx->models;

    /* storing - enlarge the list of modules being currently parsed */
    ++models->parsing_sub_modules_count;
    models->parsing_sub_modules = ly_realloc(models->parsing_sub_modules,
                                             models->parsing_sub_modules_count * sizeof *models->parsing_sub_modules);
    LY_CHECK_ERR_RETURN(!models->parsing_sub_modules, LOGMEM(module->ctx), -1);
    models->parsing_sub_modules[models->parsing_sub_modules_count - 1] = module;

    return 0;
}