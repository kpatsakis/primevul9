WandExport MagickBooleanType DrawPopPattern(DrawingWand *wand)
{
  char
    geometry[MagickPathExtent],
    key[MagickPathExtent];

  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  if (wand->image == (Image *) NULL)
    ThrowDrawException(WandError,"ContainsNoImages",wand->name);
  if (wand->pattern_id == (const char *) NULL)
    {
      ThrowDrawException(DrawWarning,"NotCurrentlyPushingPatternDefinition",
        wand->name);
      return(MagickFalse);
    }
  (void) FormatLocaleString(key,MagickPathExtent,"%s",wand->pattern_id);
  (void) SetImageArtifact(wand->image,key,wand->mvg+wand->pattern_offset);
  (void) FormatLocaleString(geometry,MagickPathExtent,"%.20gx%.20g%+.20g%+.20g",
    (double) wand->pattern_bounds.width,(double) wand->pattern_bounds.height,
    (double) wand->pattern_bounds.x,(double) wand->pattern_bounds.y);
  (void) SetImageArtifact(wand->image,key,geometry);
  wand->pattern_id=DestroyString(wand->pattern_id);
  wand->pattern_offset=0;
  wand->pattern_bounds.x=0;
  wand->pattern_bounds.y=0;
  wand->pattern_bounds.width=0;
  wand->pattern_bounds.height=0;
  wand->filter_off=MagickTrue;
  if (wand->indent_depth > 0)
    wand->indent_depth--;
  (void) MVGPrintf(wand,"pop pattern\n");
  return(MagickTrue);
}