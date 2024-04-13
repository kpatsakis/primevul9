WandExport void DrawPathEllipticArcRelative(DrawingWand *wand,const double rx,
  const double ry,const double x_axis_rotation,
  const MagickBooleanType large_arc_flag,const MagickBooleanType sweep_flag,
  const double x,const double y)
{
  DrawPathEllipticArc(wand,RelativePathMode,rx,ry,x_axis_rotation,
    large_arc_flag,sweep_flag,x,y);
}