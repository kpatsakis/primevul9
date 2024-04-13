WandExport MagickBooleanType DrawSetFont(DrawingWand *wand,
  const char *font_name)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  assert(font_name != (const char *) NULL);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->font == (char *) NULL) ||
      (LocaleCompare(CurrentContext->font,font_name) != 0))
    {
      (void) CloneString(&CurrentContext->font,font_name);
      (void) MVGPrintf(wand,"font '%s'\n",font_name);
    }
  return(MagickTrue);
}