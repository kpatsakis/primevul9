WandExport void DrawSetStrokeMiterLimit(DrawingWand *wand,
  const size_t miterlimit)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if (CurrentContext->miterlimit != miterlimit)
    {
      CurrentContext->miterlimit=miterlimit;
      (void) MVGPrintf(wand,"stroke-miterlimit %.20g\n",(double) miterlimit);
    }
}