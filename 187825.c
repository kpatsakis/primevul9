WandExport MagickBooleanType DrawSetFontFamily(DrawingWand *wand,
  const char *font_family)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  assert(font_family != (const char *) NULL);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->family == (const char *) NULL) ||
      (LocaleCompare(CurrentContext->family,font_family) != 0))
    {
      (void) CloneString(&CurrentContext->family,font_family);
      (void) MVGPrintf(wand,"font-family '%s'\n",font_family);
    }
  return(MagickTrue);
}