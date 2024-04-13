WandExport void DrawPolyline(DrawingWand *wand,
  const size_t number_coordinates,const PointInfo *coordinates)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  MVGAppendPointsCommand(wand,"polyline",number_coordinates,coordinates);
}