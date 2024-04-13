jbig2_image_new(Jbig2Ctx *ctx, uint32_t width, uint32_t height)
{
    Jbig2Image *image;
    uint32_t stride;

    if (width == 0 || height == 0) {
        jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "failed to create zero sized image");
        return NULL;
    }

    image = jbig2_new(ctx, Jbig2Image, 1);
    if (image == NULL) {
        jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "failed to allocate image");
        return NULL;
    }

    stride = ((width - 1) >> 3) + 1;    /* generate a byte-aligned stride */

    /* check for integer multiplication overflow */
    if (height > (INT32_MAX / stride)) {
        jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "integer multiplication overflow (stride=%u, height=%u)", stride, height);
        jbig2_free(ctx->allocator, image);
        return NULL;
    }
    image->data = jbig2_new(ctx, uint8_t, (size_t) height * stride);
    if (image->data == NULL) {
        jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "failed to allocate image data buffer (stride=%u, height=%u)", stride, height);
        jbig2_free(ctx->allocator, image);
        return NULL;
    }

    image->width = width;
    image->height = height;
    image->stride = stride;
    image->refcount = 1;

    return image;
}