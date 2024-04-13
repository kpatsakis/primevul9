WandExport DrawInfo *PeekDrawingWand(const DrawingWand *wand)
{
  DrawInfo
    *draw_info;

  assert(wand != (const DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  draw_info=CloneDrawInfo((ImageInfo *) NULL,CurrentContext);
  GetAffineMatrix(&draw_info->affine);
  (void) CloneString(&draw_info->primitive,wand->mvg);
  return(draw_info);
}