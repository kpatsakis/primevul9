int qemu_show_nic_models(const char *arg, const char *const *models)
{
    int i;

    if (!arg || !is_help_option(arg)) {
        return 0;
    }

    printf("Supported NIC models:\n");
    for (i = 0 ; models[i]; i++) {
        printf("%s\n", models[i]);
    }
    return 1;
}