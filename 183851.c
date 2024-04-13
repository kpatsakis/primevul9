yang_read_leafref_path(struct lys_module *module, struct yang_type *stype, char *value)
{
    if (stype->base && (stype->base != LY_TYPE_LEAFREF)) {
        LOGVAL(module->ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "require-instance");
        goto error;
    }
    if (stype->type->info.lref.path) {
        LOGVAL(module->ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, "path", "type");
        goto error;
    }
    stype->type->info.lref.path = lydict_insert_zc(module->ctx, value);
    stype->base = LY_TYPE_LEAFREF;
    return EXIT_SUCCESS;

error:
    free(value);
    return EXIT_FAILURE;
}