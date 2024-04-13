WandExport void DrawSetStrokeLineJoin(DrawingWand *wand,const LineJoin linejoin)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->linejoin != linejoin))
    {
      CurrentContext->linejoin=linejoin;
      (void) MVGPrintf(wand, "stroke-linejoin '%s'\n",CommandOptionToMnemonic(
        MagickLineJoinOptions,(ssize_t) linejoin));
    }
}