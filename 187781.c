WandExport double DrawGetFillOpacity(const DrawingWand *wand)
{
  double
    alpha;

  assert(wand != (const DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  alpha=(double) QuantumScale*CurrentContext->fill.alpha;
  return(alpha);
}