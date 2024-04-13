lyp_check_circmod(struct lys_module *module, const char *value, int type)
{
    LY_ECODE code = type ? LYE_CIRC_IMPORTS : LYE_CIRC_INCLUDES;
    struct ly_modules_list *models = &module->ctx->models;
    uint8_t i;

    /* include/import itself */
    if (ly_strequal(module->name, value, 1)) {
        LOGVAL(module->ctx, code, LY_VLOG_NONE, NULL, value);
        return -1;
    }

    /* currently parsed modules */
    for (i = 0; i < models->parsing_sub_modules_count; i++) {
        if (ly_strequal(models->parsing_sub_modules[i]->name, value, 1)) {
            LOGVAL(module->ctx, code, LY_VLOG_NONE, NULL, value);
            return -1;
        }
    }

    return 0;
}