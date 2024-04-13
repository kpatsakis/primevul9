WandExport void DrawBezier(DrawingWand *wand,
  const size_t number_coordinates,const PointInfo *coordinates)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  assert(coordinates != (const PointInfo *) NULL);
  MVGAppendPointsCommand(wand,"bezier",number_coordinates,coordinates);
}