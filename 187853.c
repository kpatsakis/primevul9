WandExport void DrawPathCurveToQuadraticBezierAbsolute(DrawingWand *wand,
  const double x1,const double y1,const double x,const double y)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  DrawPathCurveToQuadraticBezier(wand,AbsolutePathMode,x1,y1,x,y);
}