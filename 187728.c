WandExport void DrawPathLineToVerticalRelative(DrawingWand *wand,const double y)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  DrawPathLineToVertical(wand,RelativePathMode,y);
}