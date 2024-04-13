WandExport MagickBooleanType DrawPushPattern(DrawingWand *wand,
  const char *pattern_id,const double x,const double y,const double width,
  const double height)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  assert(pattern_id != (const char *) NULL);
  if (wand->pattern_id != NULL)
    {
      ThrowDrawException(DrawError,"AlreadyPushingPatternDefinition",
        wand->pattern_id);
      return(MagickFalse);
    }
  wand->filter_off=MagickTrue;
  (void) MVGPrintf(wand,"push pattern %s %.20g %.20g %.20g %.20g\n",pattern_id,
    x,y,width,height);
  wand->indent_depth++;
  wand->pattern_id=AcquireString(pattern_id);
  wand->pattern_bounds.x=(ssize_t) ceil(x-0.5);
  wand->pattern_bounds.y=(ssize_t) ceil(y-0.5);
  wand->pattern_bounds.width=(size_t) floor(width+0.5);
  wand->pattern_bounds.height=(size_t) floor(height+0.5);
  wand->pattern_offset=wand->mvg_length;
  return(MagickTrue);
}