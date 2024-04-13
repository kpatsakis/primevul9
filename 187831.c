WandExport void DrawSetStrokeLineCap(DrawingWand *wand,const LineCap linecap)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) || (CurrentContext->linecap != linecap))
    {
      CurrentContext->linecap=linecap;
      (void) MVGPrintf(wand,"stroke-linecap '%s'\n",CommandOptionToMnemonic(
        MagickLineCapOptions,(ssize_t) linecap));
    }
}