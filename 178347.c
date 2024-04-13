jbig2_image_release(Jbig2Ctx *ctx, Jbig2Image *image)
{
    if (image == NULL)
        return;
    image->refcount--;
    if (image->refcount == 0)
        jbig2_image_free(ctx, image);
}