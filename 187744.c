WandExport MagickBooleanType DrawGetFontResolution(const DrawingWand *wand,
  double *x,double *y)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  *x=72.0;
  *y=72.0;
  if (CurrentContext->density != (char *) NULL)
    {
      GeometryInfo
        geometry_info;

      MagickStatusType
        flags;

      flags=ParseGeometry(CurrentContext->density,&geometry_info);
      *x=geometry_info.rho;
      *y=geometry_info.sigma;
      if ((flags & SigmaValue) == MagickFalse)
        *y=(*x);
    }
  return(MagickTrue);
}