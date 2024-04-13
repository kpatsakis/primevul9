int qemu_find_nic_model(NICInfo *nd, const char * const *models,
                        const char *default_model)
{
    int i;

    if (!nd->model)
        nd->model = g_strdup(default_model);

    for (i = 0 ; models[i]; i++) {
        if (strcmp(nd->model, models[i]) == 0)
            return i;
    }

    error_report("Unsupported NIC model: %s", nd->model);
    return -1;
}