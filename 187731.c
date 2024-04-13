WandExport DrawingWand *NewDrawingWand(void)
{
  const char
    *quantum;

  DrawingWand
    *wand;

  size_t
    depth;

  quantum=GetMagickQuantumDepth(&depth);
  if (depth != MAGICKCORE_QUANTUM_DEPTH)
    ThrowWandFatalException(WandError,"QuantumDepthMismatch",quantum);
  wand=(DrawingWand *) AcquireMagickMemory(sizeof(*wand));
  if (wand == (DrawingWand *) NULL)
    ThrowWandFatalException(ResourceLimitFatalError,"MemoryAllocationFailed",
      GetExceptionMessage(errno));
  (void) ResetMagickMemory(wand,0,sizeof(*wand));
  wand->id=AcquireWandId();
  (void) FormatLocaleString(wand->name,MagickPathExtent,"%s-%.20g",
    DrawingWandId,(double) wand->id);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
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
  wand->graphic_context=(DrawInfo **) AcquireMagickMemory(sizeof(
    *wand->graphic_context));
  if (wand->graphic_context == (DrawInfo **) NULL)
    ThrowWandFatalException(ResourceLimitFatalError,"MemoryAllocationFailed",
      GetExceptionMessage(errno));
  wand->filter_off=MagickTrue;
  wand->indent_depth=0;
  wand->path_operation=PathDefaultOperation;
  wand->path_mode=DefaultPathMode;
  wand->exception=AcquireExceptionInfo();
  wand->image=AcquireImage((const ImageInfo *) NULL,wand->exception);
  wand->destroy=MagickTrue;
  wand->debug=IsEventLogging();
  wand->signature=MagickWandSignature;
  CurrentContext=CloneDrawInfo((ImageInfo *) NULL,(DrawInfo *) NULL);
  return(wand);
}