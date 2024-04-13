WandExport void DrawPushClipPath(DrawingWand *wand,const char *clip_mask_id)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  assert(clip_mask_id != (const char *) NULL);
  (void) MVGPrintf(wand,"push clip-path %s\n",clip_mask_id);
  wand->indent_depth++;
}