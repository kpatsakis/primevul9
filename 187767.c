WandExport void DrawSetStrokeDashOffset(DrawingWand *wand,
  const double dash_offset)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) ||
     (fabs(CurrentContext->dash_offset-dash_offset) >= MagickEpsilon))
    {
      CurrentContext->dash_offset=dash_offset;
      (void) MVGPrintf(wand,"stroke-dashoffset %.20g\n",dash_offset);
    }
}