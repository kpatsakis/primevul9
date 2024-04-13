WandExport MagickBooleanType DrawSetStrokePatternURL(DrawingWand *wand,
  const char *stroke_url)
{
  char
    pattern[MagickPathExtent],
    pattern_spec[MagickPathExtent];

  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if (wand->image == (Image *) NULL)
    ThrowDrawException(WandError,"ContainsNoImages",wand->name);
  assert(stroke_url != NULL);
  if (stroke_url[0] != '#')
    ThrowDrawException(DrawError,"NotARelativeURL",stroke_url);
  (void) FormatLocaleString(pattern,MagickPathExtent,"%s",stroke_url+1);
  if (GetImageArtifact(wand->image,pattern) == (const char *) NULL)
    {
      ThrowDrawException(DrawError,"URLNotFound",stroke_url)
      return(MagickFalse);
    }
  (void) FormatLocaleString(pattern_spec,MagickPathExtent,"url(%s)",stroke_url);
#if DRAW_BINARY_IMPLEMENTATION
  DrawPatternPath(wand->image,CurrentContext,pattern_spec,
    &CurrentContext->stroke_pattern);
#endif
  if (CurrentContext->stroke.alpha != (Quantum) TransparentAlpha)
    CurrentContext->stroke.alpha=(double) CurrentContext->alpha;
  (void) MVGPrintf(wand,"stroke %s\n",pattern_spec);
  return(MagickTrue);
}