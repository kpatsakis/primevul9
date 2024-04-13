WandExport MagickBooleanType PopDrawingWand(DrawingWand *wand)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if (wand->index == 0)
    {
      ThrowDrawException(DrawError,"UnbalancedGraphicContextPushPop",wand->name)
      return(MagickFalse);
    }
  /*
    Destroy clip path if not same in preceding wand.
  */
#if DRAW_BINARY_IMPLEMENTATION
  if (wand->image == (Image *) NULL)
    ThrowDrawException(WandError,"ContainsNoImages",wand->name);
  if (CurrentContext->clip_mask != (char *) NULL)
    if (LocaleCompare(CurrentContext->clip_mask,
        wand->graphic_context[wand->index-1]->clip_mask) != 0)
      (void) SetImageMask(wand->image,ReadPixelMask,(Image *) NULL,
        wand->exception);
#endif
  CurrentContext=DestroyDrawInfo(CurrentContext);
  wand->index--;
  if (wand->indent_depth > 0)
    wand->indent_depth--;
  (void) MVGPrintf(wand,"pop graphic-context\n");
  return(MagickTrue);
}