lyp_check_include_missing_recursive(struct lys_module *main_module, struct lys_submodule *sub)
{
    uint8_t i, j;
    void *reallocated;
    int ret = 0, tmp;
    struct ly_ctx *ctx = main_module->ctx;

    for (i = 0; i < sub->inc_size; i++) {
        /* check that the include is also present in the main module */
        for (j = 0; j < main_module->inc_size; j++) {
            if (main_module->inc[j].submodule == sub->inc[i].submodule) {
                break;
            }
        }

        if (j == main_module->inc_size) {
            /* match not found */
            if (main_module->version >= LYS_VERSION_1_1) {
                LOGVAL(ctx, LYE_MISSSTMT, LY_VLOG_NONE, NULL, "include");
                LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL,
                       "The main module \"%s\" misses include of the \"%s\" submodule used in another submodule \"%s\".",
                       main_module->name, sub->inc[i].submodule->name, sub->name);
                /* now we should return error, but due to the issues with freeing the module, we actually have
                 * to go through the all includes and, as in case of 1.0, add them into the main module and fail
                 * at the end when all the includes are in the main module and we can free them */
                ret = 1;
            } else {
                /* not strictly an error in YANG 1.0 */
                LOGWRN(ctx, "The main module \"%s\" misses include of the \"%s\" submodule used in another submodule \"%s\".",
                       main_module->name, sub->inc[i].submodule->name, sub->name);
                LOGWRN(ctx, "To avoid further issues, adding submodule \"%s\" into the main module \"%s\".",
                       sub->inc[i].submodule->name, main_module->name);
                /* but since it is a good practise and because we expect all the includes in the main module
                 * when searching it and also when freeing the module, put it into it */
            }
            main_module->inc_size++;
            reallocated = realloc(main_module->inc, main_module->inc_size * sizeof *main_module->inc);
            LY_CHECK_ERR_RETURN(!reallocated, LOGMEM(ctx), 1);
            main_module->inc = reallocated;
            memset(&main_module->inc[main_module->inc_size - 1], 0, sizeof *main_module->inc);
            /* to avoid unexpected consequences, copy just a link to the submodule and the revision,
             * all other substatements of the include are ignored */
            memcpy(&main_module->inc[main_module->inc_size - 1].rev, sub->inc[i].rev, LY_REV_SIZE - 1);
            main_module->inc[main_module->inc_size - 1].submodule = sub->inc[i].submodule;
        }

        /* recursion */
        tmp = lyp_check_include_missing_recursive(main_module, sub->inc[i].submodule);
        if (!ret && tmp) {
            ret = 1;
        }
    }

    return ret;
}