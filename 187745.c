WandExport void DrawSetStrokeWidth(DrawingWand *wand,const double stroke_width)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) ||
      (fabs(CurrentContext->stroke_width-stroke_width) >= MagickEpsilon))
    {
      CurrentContext->stroke_width=stroke_width;
      (void) MVGPrintf(wand,"stroke-width %.20g\n",stroke_width);
    }
}