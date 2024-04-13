WandExport MagickBooleanType PushDrawingWand(DrawingWand *wand)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  wand->index++;
  wand->graphic_context=(DrawInfo **) ResizeQuantumMemory(wand->graphic_context,
    (size_t) wand->index+1UL,sizeof(*wand->graphic_context));
  if (wand->graphic_context == (DrawInfo **) NULL)
    {
      wand->index--;
      ThrowDrawException(ResourceLimitError,"MemoryAllocationFailed",
        wand->name);
      return(MagickFalse);
    }
  CurrentContext=CloneDrawInfo((ImageInfo *) NULL,
    wand->graphic_context[wand->index-1]);
  (void) MVGPrintf(wand,"push graphic-context\n");
  wand->indent_depth++;
  return(MagickTrue);
}