WandExport void DrawSetOpacity(DrawingWand *wand,const double opacity)
{
  Quantum
    quantum_alpha;

  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  quantum_alpha=ClampToQuantum(QuantumRange*opacity);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->alpha != quantum_alpha))
    {
      CurrentContext->alpha=quantum_alpha;
      (void) MVGPrintf(wand,"opacity %.20g\n",opacity);
    }
}