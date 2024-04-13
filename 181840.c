dup_prefix_check(const char *prefix, struct lys_module *module)
{
    int i;

    if (module->prefix && !strcmp(module->prefix, prefix)) {
        return EXIT_FAILURE;
    }
    for (i = 0; i < module->imp_size; i++) {
        if (!strcmp(module->imp[i].prefix, prefix)) {
            return EXIT_FAILURE;
        }
    }

    return EXIT_SUCCESS;
}