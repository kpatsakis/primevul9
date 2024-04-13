lyp_check_circmod_pop(struct ly_ctx *ctx)
{
    if (!ctx->models.parsing_sub_modules_count) {
        LOGINT(ctx);
        return;
    }

    /* update the list of currently being parsed modules */
    ctx->models.parsing_sub_modules_count--;
    if (!ctx->models.parsing_sub_modules_count) {
        free(ctx->models.parsing_sub_modules);
        ctx->models.parsing_sub_modules = NULL;
    }
}