WandExport DrawingWand *DestroyDrawingWand(DrawingWand *wand)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  for ( ; wand->index > 0; wand->index--)
    CurrentContext=DestroyDrawInfo(CurrentContext);
  CurrentContext=DestroyDrawInfo(CurrentContext);
  wand->graphic_context=(DrawInfo **) RelinquishMagickMemory(
    wand->graphic_context);
  if (wand->pattern_id != (char *) NULL)
    wand->pattern_id=DestroyString(wand->pattern_id);
  wand->mvg=DestroyString(wand->mvg);
  if ((wand->destroy != MagickFalse) && (wand->image != (Image *) NULL))
    wand->image=DestroyImage(wand->image);
  wand->image=(Image *) NULL;
  wand->exception=DestroyExceptionInfo(wand->exception);
  wand->signature=(~MagickWandSignature);
  RelinquishWandId(wand->id);
  wand=(DrawingWand *) RelinquishMagickMemory(wand);
  return(wand);
}