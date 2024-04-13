MagickExport void GetPixelInfo(const Image *image,PixelInfo *pixel)
{
  (void) memset(pixel,0,sizeof(*pixel));
  pixel->storage_class=DirectClass;
  pixel->colorspace=sRGBColorspace;
  pixel->depth=MAGICKCORE_QUANTUM_DEPTH;
  pixel->alpha_trait=UndefinedPixelTrait;
  pixel->alpha=(double) OpaqueAlpha;
  if (image == (const Image *) NULL)
    return;
  pixel->storage_class=image->storage_class;
  pixel->colorspace=image->colorspace;
  pixel->alpha_trait=image->alpha_trait;
  pixel->depth=image->depth;
  pixel->fuzz=image->fuzz;
}