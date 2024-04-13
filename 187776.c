WandExport DrawingWand *CloneDrawingWand(const DrawingWand *wand)
{
  DrawingWand
    *clone_wand;

  register ssize_t
    i;

  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  clone_wand=(DrawingWand *) AcquireMagickMemory(sizeof(*clone_wand));
  if (clone_wand == (DrawingWand *) NULL)
    ThrowWandFatalException(ResourceLimitFatalError,
      "MemoryAllocationFailed",GetExceptionMessage(errno));
  (void) ResetMagickMemory(clone_wand,0,sizeof(*clone_wand));
  clone_wand->id=AcquireWandId();
  (void) FormatLocaleString(clone_wand->name,MagickPathExtent,
    "DrawingWand-%.20g",(double) clone_wand->id);
  clone_wand->exception=AcquireExceptionInfo();
  InheritException(clone_wand->exception,wand->exception);
  clone_wand->mvg=AcquireString(wand->mvg);
  clone_wand->mvg_length=strlen(clone_wand->mvg);
  clone_wand->mvg_alloc=wand->mvg_length+1;
  clone_wand->mvg_width=wand->mvg_width;
  clone_wand->pattern_id=AcquireString(wand->pattern_id);
  clone_wand->pattern_offset=wand->pattern_offset;
  clone_wand->pattern_bounds=wand->pattern_bounds;
  clone_wand->index=wand->index;
  clone_wand->graphic_context=(DrawInfo **) AcquireQuantumMemory((size_t)
    wand->index+1UL,sizeof(*wand->graphic_context));
  if (clone_wand->graphic_context == (DrawInfo **) NULL)
    ThrowWandFatalException(ResourceLimitFatalError,"MemoryAllocationFailed",
      GetExceptionMessage(errno));
  for (i=0; i <= (ssize_t) wand->index; i++)
    clone_wand->graphic_context[i]=CloneDrawInfo((ImageInfo *) NULL,
      wand->graphic_context[i]);
  clone_wand->filter_off=wand->filter_off;
  clone_wand->indent_depth=wand->indent_depth;
  clone_wand->path_operation=wand->path_operation;
  clone_wand->path_mode=wand->path_mode;
  clone_wand->image=wand->image;
  if (wand->image != (Image *) NULL)
    clone_wand->image=CloneImage(wand->image,0,0,MagickTrue,
      clone_wand->exception);
  clone_wand->destroy=MagickTrue;
  clone_wand->debug=IsEventLogging();
  if (clone_wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",clone_wand->name);
  clone_wand->signature=MagickWandSignature;
  return(clone_wand);
}