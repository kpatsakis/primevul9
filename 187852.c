static void MVGAppendColor(DrawingWand *wand,const PixelInfo *packet)
{
  if ((packet->red == 0) && (packet->green == 0) && (packet->blue == 0) &&
      (packet->alpha == (Quantum) TransparentAlpha))
    (void) MVGPrintf(wand,"none");
  else
    {
      char
        tuple[MagickPathExtent];

      PixelInfo
        pixel;

      GetPixelInfo(wand->image,&pixel);
      pixel.colorspace=sRGBColorspace;
      pixel.alpha_trait=packet->alpha != OpaqueAlpha ? BlendPixelTrait :
        UndefinedPixelTrait;
      pixel.red=(double) packet->red;
      pixel.green=(double) packet->green;
      pixel.blue=(double) packet->blue;
      pixel.alpha=(double) packet->alpha;
      GetColorTuple(&pixel,MagickTrue,tuple);
      (void) MVGPrintf(wand,"%s",tuple);
    }
}