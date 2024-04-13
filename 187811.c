WandExport void DrawPathCurveToRelative(DrawingWand *wand,const double x1,
  const double y1,const double x2,const double y2,const double x,const double y)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  DrawPathCurveTo(wand,RelativePathMode,x1,y1,x2,y2,x,y);
}