WandExport void DrawPathCurveToSmoothRelative(DrawingWand *wand,const double x2,
  const double y2,const double x,const double y)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  DrawPathCurveToSmooth(wand,RelativePathMode,x2,y2,x,y);
}