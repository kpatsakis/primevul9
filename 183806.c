yang_read_deviate_unsupported(struct ly_ctx *ctx, struct lys_deviation *dev)
{
    if (dev->deviate_size) {
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "\"not-supported\" deviation cannot be combined with any other deviation.");
        return NULL;
    }
    dev->deviate = calloc(1, sizeof *dev->deviate);
    LY_CHECK_ERR_RETURN(!dev->deviate, LOGMEM(ctx), NULL);
    dev->deviate[dev->deviate_size].mod = LY_DEVIATE_NO;
    dev->deviate_size = 1;
    return dev->deviate;
}