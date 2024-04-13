WandExport void DrawSetTextEncoding(DrawingWand *wand,const char *encoding)
{
  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  assert(encoding != (char *) NULL);
  if ((wand->filter_off != MagickFalse) ||
      (CurrentContext->encoding == (char *) NULL) ||
      (LocaleCompare(CurrentContext->encoding,encoding) != 0))
    {
      (void) CloneString(&CurrentContext->encoding,encoding);
      (void) MVGPrintf(wand,"encoding '%s'\n",encoding);
    }
}