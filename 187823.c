WandExport void DrawPathCurveToQuadraticBezierSmoothRelative(DrawingWand *wand,
  const double x,const double y)
{
  DrawPathCurveToQuadraticBezierSmooth(wand,RelativePathMode,x,y);
}