static void DrawPathLineToHorizontal(DrawingWand *wand,const PathMode mode,
  const double x)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->path_operation != PathLineToHorizontalOperation) ||
      (wand->path_mode != mode))
    {
      wand->path_operation=PathLineToHorizontalOperation;
      wand->path_mode=mode;
      (void) MVGAutoWrapPrintf(wand,"%c%.20g",mode == AbsolutePathMode ?
        'H' : 'h',x);
    }
  else
    (void) MVGAutoWrapPrintf(wand," %.20g",x);
}