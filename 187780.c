static void DrawPathCurveToQuadraticBezier(DrawingWand *wand,
  const PathMode mode,const double x1,double y1,const double x,const double y)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->path_operation != PathCurveToQuadraticBezierOperation) ||
      (wand->path_mode != mode))
    {
      wand->path_operation=PathCurveToQuadraticBezierOperation;
      wand->path_mode=mode;
      (void) MVGAutoWrapPrintf(wand, "%c%.20g %.20g %.20g %.20g",
         mode == AbsolutePathMode ? 'Q' : 'q',x1,y1,x,y);
    }
  else
    (void) MVGAutoWrapPrintf(wand," %.20g %.20g %.20g %.20g",x1,y1,x,y);
}