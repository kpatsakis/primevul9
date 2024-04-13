WandExport void DrawPathLineToVerticalAbsolute(DrawingWand *wand,const double y)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  DrawPathLineToVertical(wand,AbsolutePathMode,y);
}