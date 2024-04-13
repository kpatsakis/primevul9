WandExport void DrawResetVectorGraphics(DrawingWand *wand)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if (wand->mvg != (char *) NULL)
    wand->mvg=DestroyString(wand->mvg);
  wand->mvg_alloc=0;
  wand->mvg_length=0;
  wand->mvg_width=0;
}