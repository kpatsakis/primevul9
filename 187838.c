static void DrawPathEllipticArc(DrawingWand *wand, const PathMode mode,
  const double rx,const double ry,const double x_axis_rotation,
  const MagickBooleanType large_arc_flag,const MagickBooleanType sweep_flag,
  const double x,const double y)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if ((wand->path_operation != PathEllipticArcOperation) ||
      (wand->path_mode != mode))
    {
      wand->path_operation=PathEllipticArcOperation;
      wand->path_mode=mode;
      (void) MVGAutoWrapPrintf(wand, "%c%.20g %.20g %.20g %u %u %.20g %.20g",
        mode == AbsolutePathMode ? 'A' : 'a',rx,ry,x_axis_rotation,
        large_arc_flag,sweep_flag,x,y);
    }
  else
    (void) MVGAutoWrapPrintf(wand," %.20g %.20g %.20g %u %u %.20g %.20g",rx,ry,
      x_axis_rotation,large_arc_flag,sweep_flag,x,y);
}