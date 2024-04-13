void qemu_check_nic_model(NICInfo *nd, const char *model)
{
    const char *models[2];

    models[0] = model;
    models[1] = NULL;

    if (qemu_show_nic_models(nd->model, models))
        exit(0);
    if (qemu_find_nic_model(nd, models, model) < 0)
        exit(1);
}