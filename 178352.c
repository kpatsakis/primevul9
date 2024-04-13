jbig2_image_resize(Jbig2Ctx *ctx, Jbig2Image *image, uint32_t width, uint32_t height, int value)
{
    if (width == image->width) {
        uint8_t *data;

        /* check for integer multiplication overflow */
        if (image->height > (INT32_MAX / image->stride)) {
            jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "integer multiplication overflow during resize (stride=%u, height=%u)", image->stride, height);
            return NULL;
        }
        /* use the same stride, just change the length */
        data = jbig2_renew(ctx, image->data, uint8_t, (size_t) height * image->stride);
        if (data == NULL) {
            jbig2_error(ctx, JBIG2_SEVERITY_FATAL, -1, "failed to reallocate image");
            return NULL;
        }
        image->data = data;
        if (height > image->height) {
            const uint8_t fill = value ? 0xFF : 0x00;
            memset(image->data + (size_t) image->height * image->stride, fill, ((size_t) height - image->height) * image->stride);
        }
        image->height = height;

    } else {
        Jbig2Image *newimage;
        int code;

        /* Unoptimized implementation, but it works. */

        newimage = jbig2_image_new(ctx, width, height);
        if (newimage == NULL) {
            jbig2_error(ctx, JBIG2_SEVERITY_WARNING, -1, "failed to allocate resized image");
            return NULL;
        }
        jbig2_image_clear(ctx, newimage, value);

        code = jbig2_image_compose(ctx, newimage, image, 0, 0, JBIG2_COMPOSE_REPLACE);
        if (code < 0) {
            jbig2_error(ctx, JBIG2_SEVERITY_WARNING, -1, "failed to compose image buffers when resizing");
            jbig2_image_release(ctx, newimage);
            return NULL;
        }

        /* if refcount > 1 the original image, its pointer must
        be kept, so simply replaces its innards, and throw away
        the empty new image shell. */
        jbig2_free(ctx->allocator, image->data);
        image->width = newimage->width;
        image->height = newimage->height;
        image->stride = newimage->stride;
        image->data = newimage->data;
        jbig2_free(ctx->allocator, newimage);
    }

    return image;
}