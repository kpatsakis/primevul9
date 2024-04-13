WandExport void ClearDrawingWand(DrawingWand *wand)
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
  else
    wand->image=(Image *) NULL;
  wand->mvg=(char *) NULL;
  wand->mvg_alloc=0;
  wand->mvg_length=0;
  wand->mvg_width=0;
  wand->pattern_id=(char *) NULL;
  wand->pattern_offset=0;
  wand->pattern_bounds.x=0;
  wand->pattern_bounds.y=0;
  wand->pattern_bounds.width=0;
  wand->pattern_bounds.height=0;
  wand->index=0;
  wand->graphic_context=(DrawInfo **) AcquireMagickMemory(
    sizeof(*wand->graphic_context));
  if (wand->graphic_context == (DrawInfo **) NULL)
    {
      ThrowDrawException(ResourceLimitError,"MemoryAllocationFailed",
        wand->name);
      return;
    }
  CurrentContext=CloneDrawInfo((ImageInfo *) NULL,(DrawInfo *) NULL);
  wand->filter_off=MagickTrue;
  wand->indent_depth=0;
  wand->path_operation=PathDefaultOperation;
  wand->path_mode=DefaultPathMode;
  wand->image=AcquireImage((const ImageInfo *) NULL,wand->exception);
  ClearMagickException(wand->exception);
  wand->destroy=MagickTrue;
  wand->debug=IsEventLogging();
}