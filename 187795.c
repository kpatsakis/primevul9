WandExport void DrawPopDefs(DrawingWand *wand)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if (wand->indent_depth > 0)
    wand->indent_depth--;
  (void) MVGPrintf(wand,"pop defs\n");
}