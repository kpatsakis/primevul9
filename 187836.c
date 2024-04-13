WandExport double DrawGetStrokeOpacity(const DrawingWand *wand)
{
  double
    alpha;

  assert(wand != (const DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  alpha=(double) QuantumScale*CurrentContext->stroke.alpha;
  return(alpha);
}