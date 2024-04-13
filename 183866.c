yang_read_deviate(struct ly_ctx *ctx, struct lys_deviation *dev, LYS_DEVIATE_TYPE mod)
{
    struct lys_deviate *deviate;

    if (dev->deviate_size && dev->deviate[0].mod == LY_DEVIATE_NO) {
        LOGVAL(ctx, LYE_INSTMT, LY_VLOG_NONE, NULL, "not-supported");
        LOGVAL(ctx, LYE_SPEC, LY_VLOG_NONE, NULL, "\"not-supported\" deviation cannot be combined with any other deviation.");
        return NULL;
    }
    if (!(dev->deviate_size % LY_YANG_ARRAY_SIZE)) {
        deviate = realloc(dev->deviate, (LY_YANG_ARRAY_SIZE + dev->deviate_size) * sizeof *deviate);
        LY_CHECK_ERR_RETURN(!deviate, LOGMEM(ctx), NULL);
        memset(deviate + dev->deviate_size, 0, LY_YANG_ARRAY_SIZE * sizeof *deviate);
        dev->deviate = deviate;
    }
    dev->deviate[dev->deviate_size].mod = mod;
    return &dev->deviate[dev->deviate_size++];
}