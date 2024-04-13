WandExport MagickBooleanType DrawSetFontResolution(DrawingWand *wand,
  const double x_resolution,const double y_resolution)
{
  char
    density[MagickPathExtent];

  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  (void) FormatLocaleString(density,MagickPathExtent,"%.20gx%.20g",x_resolution,
    y_resolution);
  (void) CloneString(&CurrentContext->density,density);
  return(MagickTrue);
}