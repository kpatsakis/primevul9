jbig2_image_reference(Jbig2Ctx *ctx, Jbig2Image *image)
{
    if (image)
        image->refcount++;
    return image;
}