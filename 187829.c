WandExport MagickBooleanType DrawSetClipPath(DrawingWand *wand,
  const char *clip_mask)
{
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",clip_mask);
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  assert(clip_mask != (const char *) NULL);
  if ((CurrentContext->clip_mask == (const char *) NULL) ||
      (wand->filter_off != MagickFalse) ||
      (LocaleCompare(CurrentContext->clip_mask,clip_mask) != 0))
    {
      (void) CloneString(&CurrentContext->clip_mask,clip_mask);
#if DRAW_BINARY_IMPLEMENTATION
      if (wand->image == (Image *) NULL)
        ThrowDrawException(WandError,"ContainsNoImages",wand->name);
      (void) DrawClipPath(wand->image,CurrentContext,CurrentContext->clip_mask);
#endif
      (void) MVGPrintf(wand,"clip-path url(#%s)\n",clip_mask);
    }
  return(MagickTrue);
}