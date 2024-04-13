static void DrawPathLineToVertical(DrawingWand *wand,const PathMode mode,
  const double y)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->path_operation != PathLineToVerticalOperation) ||
      (wand->path_mode != mode))
    {
      wand->path_operation=PathLineToVerticalOperation;
      wand->path_mode=mode;
      (void) MVGAutoWrapPrintf(wand,"%c%.20g",mode == AbsolutePathMode ?
        'V' : 'v',y);
    }
  else
    (void) MVGAutoWrapPrintf(wand," %.20g",y);
}