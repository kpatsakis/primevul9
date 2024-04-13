lyp_get_yang_data_template(const struct lys_module *module, const char *yang_data_name, int yang_data_name_len)
{
    int i, j;
    const struct lys_node *ret = NULL;
    const struct lys_submodule *submodule;

    for(i = 0; i < module->ext_size; ++i) {
        if (!strcmp(module->ext[i]->def->name, "yang-data") && !strncmp(module->ext[i]->arg_value, yang_data_name, yang_data_name_len)
            && !module->ext[i]->arg_value[yang_data_name_len]) {
            ret = (struct lys_node *)module->ext[i];
            break;
        }
    }

    for(j = 0; !ret && j < module->inc_size; ++j) {
        submodule = module->inc[j].submodule;
        for(i = 0; i < submodule->ext_size; ++i) {
            if (!strcmp(submodule->ext[i]->def->name, "yang-data") && !strncmp(submodule->ext[i]->arg_value, yang_data_name, yang_data_name_len)
                && !submodule->ext[i]->arg_value[yang_data_name_len]) {
                ret = (struct lys_node *)submodule->ext[i];
                break;
            }
        }
    }

    return ret;
}