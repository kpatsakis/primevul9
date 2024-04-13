WandExport void DrawPathClose(DrawingWand *wand)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  (void) MVGAutoWrapPrintf(wand,"%s",wand->path_mode == AbsolutePathMode ?
    "Z" : "z");
}