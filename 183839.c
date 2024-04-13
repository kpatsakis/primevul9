yang_read_require_instance(struct ly_ctx *ctx, struct yang_type *stype, int req)
{
    if (stype->base && (stype->base != LY_TYPE_LEAFREF)) {
        LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "require-instance");
        return EXIT_FAILURE;
    }
    if (stype->type->info.lref.req) {
        LOGVAL(ctx, LYE_TOOMANY, LY_VLOG_NONE, NULL, "require-instance", "type");
        return EXIT_FAILURE;
    }
    stype->type->info.lref.req = req;
    stype->base = LY_TYPE_LEAFREF;
    return EXIT_SUCCESS;
}