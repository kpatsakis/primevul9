WandExport MagickBooleanType DrawSetFillPatternURL(DrawingWand *wand,
  const char *fill_url)
{
  char
    pattern[MagickPathExtent],
    pattern_spec[MagickPathExtent];

  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",fill_url);
  if (wand->image == (Image *) NULL)
    ThrowDrawException(WandError,"ContainsNoImages",wand->name);
  assert(fill_url != (const char *) NULL);
  if (*fill_url != '#')
    {
      ThrowDrawException(DrawError,"NotARelativeURL",fill_url);
      return(MagickFalse);
    }
  (void) FormatLocaleString(pattern,MagickPathExtent,"%s",fill_url+1);
  if (GetImageArtifact(wand->image,pattern) == (const char *) NULL)
    {
      ThrowDrawException(DrawError,"URLNotFound",fill_url)
      return(MagickFalse);
    }
  (void) FormatLocaleString(pattern_spec,MagickPathExtent,"url(%s)",fill_url);
#if DRAW_BINARY_IMPLEMENTATION
  DrawPatternPath(wand->image,CurrentContext,pattern_spec,
    &CurrentContext->fill_pattern);
#endif
  if (CurrentContext->fill.alpha != (Quantum) TransparentAlpha)
    CurrentContext->fill.alpha=(double) CurrentContext->alpha;
  (void) MVGPrintf(wand,"fill %s\n",pattern_spec);
  return(MagickTrue);
}