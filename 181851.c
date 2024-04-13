lyp_del_includedup(struct lys_module *mod, int free_subs)
{
    struct ly_modules_list *models = &mod->ctx->models;
    uint8_t i;

    assert(mod && !mod->type);

    if (models->parsed_submodules_count) {
        for (i = models->parsed_submodules_count - 1; models->parsed_submodules[i]->type; --i);
        if (models->parsed_submodules[i] == mod) {
            if (free_subs) {
                for (i = models->parsed_submodules_count - 1; models->parsed_submodules[i]->type; --i) {
                    lys_sub_module_remove_devs_augs((struct lys_module *)models->parsed_submodules[i]);
                    lys_submodule_module_data_free((struct lys_submodule *)models->parsed_submodules[i]);
                    lys_submodule_free((struct lys_submodule *)models->parsed_submodules[i], NULL);
                }
            }

            models->parsed_submodules_count = i;
            if (!models->parsed_submodules_count) {
                free(models->parsed_submodules);
                models->parsed_submodules = NULL;
            }
        }
    }
}