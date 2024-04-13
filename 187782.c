WandExport void DrawAlpha(DrawingWand *wand,const double x,const double y,
  const PaintMethod paint_method)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  (void) MVGPrintf(wand,"alpha %.20g %.20g '%s'\n",x,y,CommandOptionToMnemonic(
    MagickMethodOptions,(ssize_t) paint_method));
}