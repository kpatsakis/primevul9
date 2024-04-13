static MagickBooleanType RenderFreetype(Image *image,const DrawInfo *draw_info,
  const char *magick_unused(encoding),const PointInfo *offset,
  TypeMetric *metrics)
{
  (void) ThrowMagickException(&image->exception,GetMagickModule(),
    MissingDelegateWarning,"DelegateLibrarySupportNotBuiltIn","`%s' (Freetype)",
    draw_info->font != (char *) NULL ? draw_info->font : "none");
  return(RenderPostscript(image,draw_info,offset,metrics));
}