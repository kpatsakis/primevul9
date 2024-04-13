pdf14_ctx_free(pdf14_ctx *ctx)
{
    pdf14_buf *buf, *next;

    if (ctx->mask_stack) {
        /* A mask was created but was not used in this band. */
        rc_decrement(ctx->mask_stack->rc_mask, "pdf14_ctx_free");
        gs_free_object(ctx->memory,ctx->mask_stack,"pdf14_ctx_free");
    }
    for (buf = ctx->stack; buf != NULL; buf = next) {
        next = buf->saved;
        pdf14_buf_free(buf, ctx->memory);
    }
    gs_free_object (ctx->memory, ctx, "pdf14_ctx_free");
}