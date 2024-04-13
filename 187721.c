WandExport void DrawAnnotation(DrawingWand *wand,const double x,const double y,
  const unsigned char *text)
{
  char
    *escaped_text;

  assert(wand != (DrawingWand *) NULL);
  assert(wand->signature == MagickWandSignature);
  if (wand->debug != MagickFalse)
    (void) LogMagickEvent(WandEvent,GetMagickModule(),"%s",wand->name);
  assert(text != (const unsigned char *) NULL);
  escaped_text=EscapeString((const char *) text,'\'');
  if (escaped_text != (char *) NULL)
    {
      (void) MVGPrintf(wand,"text %.20g %.20g '%s'\n",x,y,escaped_text);
      escaped_text=DestroyString(escaped_text);
    }
}