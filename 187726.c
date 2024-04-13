WandExport void DrawPathEllipticArcAbsolute(DrawingWand *wand,const double rx,
  const double ry,const double x_axis_rotation,
  const MagickBooleanType large_arc_flag,const MagickBooleanType sweep_flag,
  const double x,const double y)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  DrawPathEllipticArc(wand,AbsolutePathMode,rx,ry,x_axis_rotation,
    large_arc_flag,sweep_flag,x,y);
}