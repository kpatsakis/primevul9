WandExport void DrawPolygon(DrawingWand *wand,
  const size_t number_coordinates,const PointInfo *coordinates)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  MVGAppendPointsCommand(wand,"polygon",number_coordinates,coordinates);
}