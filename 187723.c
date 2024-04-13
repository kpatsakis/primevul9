static void DrawPathCurveTo(DrawingWand *wand,const PathMode mode,
  const double x1,const double y1,const double x2,const double y2,
  const double x,const double y)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->path_operation != PathCurveToOperation) ||
      (wand->path_mode != mode))
    {
      wand->path_operation=PathCurveToOperation;
      wand->path_mode=mode;
      (void) MVGAutoWrapPrintf(wand, "%c%.20g %.20g %.20g %.20g %.20g %.20g",
        mode == AbsolutePathMode ? 'C' : 'c',x1,y1,x2,y2,x,y);
    }
  else
    (void) MVGAutoWrapPrintf(wand," %.20g %.20g %.20g %.20g %.20g %.20g",x1,y1,
      x2,y2,x,y);
}