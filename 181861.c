lyp_check_include_missing(struct lys_module *main_module)
{
    int ret = 0;
    uint8_t i;

    /* in YANG 1.1, all the submodules must be in the main module, check it even for
     * 1.0 where it will be printed as warning and the include will be added into the main module */
    for (i = 0; i < main_module->inc_size; i++) {
        if (lyp_check_include_missing_recursive(main_module, main_module->inc[i].submodule)) {
            ret = 1;
        }
    }

    return ret;
}