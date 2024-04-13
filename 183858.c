yang_check_version(struct lys_module *module, struct lys_submodule *submodule, char *value, int repeat)
{
    int ret = EXIT_SUCCESS;

    if (repeat) {
        LOGVAL(module->ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, "yang version", "module");
        ret = EXIT_FAILURE;
    } else {
        if (!strcmp(value, "1")) {
            if (submodule) {
                if (module->version > 1) {
                    LOGVAL(module->ctx, LYE_INVER, LY_VLOG_NONE, NULL);
                    ret = EXIT_FAILURE;
                 }
                submodule->version = 1;
            } else {
                module->version = 1;
            }
        } else if (!strcmp(value, "1.1")) {
            if (submodule) {
                if (module->version != 2) {
                    LOGVAL(module->ctx, LYE_INVER, LY_VLOG_NONE, NULL);
                    ret = EXIT_FAILURE;
                }
                submodule->version = 2;
            } else {
                module->version = 2;
            }
        } else {
            LOGVAL(module->ctx, LYE_INARG, LY_VLOG_NONE, NULL, value, "yang-version");
            ret = EXIT_FAILURE;
        }
    }
    free(value);
    return ret;
}