MagickExport XFontStruct *XBestFont(Display *display,
  const XResourceInfo *resource_info,const MagickBooleanType text_font)
{
  static const char
    *Fonts[]=
    {
      "-*-helvetica-medium-r-normal--12-*-*-*-*-*-iso8859-1",
      "-*-arial-medium-r-normal--12-*-*-*-*-*-iso8859-1",
      "-*-helvetica-medium-r-normal--12-*-*-*-*-*-iso8859-15",
      "-*-arial-medium-r-normal--12-*-*-*-*-*-iso8859-15",
      "-*-helvetica-medium-r-normal--12-*-*-*-*-*-*-*",
      "-*-arial-medium-r-normal--12-*-*-*-*-*-*-*",
      "variable",
      "fixed",
      (char *) NULL
    },
    *TextFonts[]=
    {
      "-*-courier-medium-r-normal-*-12-*-*-*-*-*-iso8859-1",
      "-*-courier-medium-r-normal-*-12-*-*-*-*-*-iso8859-15",
      "-*-fixed-medium-r-normal-*-12-*-*-*-*-*-*-*",
      "fixed",
      (char *) NULL
    };

  char
    *font_name;

  register const char
    **p;

  XFontStruct
    *font_info;

  font_info=(XFontStruct *) NULL;
  font_name=resource_info->font;
  if (text_font != MagickFalse)
    font_name=resource_info->text_font;
  if ((font_name != (char *) NULL) && (*font_name != '\0'))
    {
      char
        **fontlist;

      register int
        i;

      /*
        Load preferred font specified in the X resource database.
      */
      fontlist=FontToList(font_name);
      if (fontlist != (char **) NULL)
        {
          for (i=0; fontlist[i] != (char *) NULL; i++)
          {
            if (font_info == (XFontStruct *) NULL)
              font_info=XLoadQueryFont(display,fontlist[i]);
            fontlist[i]=DestroyString(fontlist[i]);
          }
          fontlist=(char **) RelinquishMagickMemory(fontlist);
        }
      if (font_info == (XFontStruct *) NULL)
        ThrowXWindowException(XServerError,"UnableToLoadFont",font_name);
    }
  /*
    Load fonts from list of fonts until one is found.
  */
  p=Fonts;
  if (text_font != MagickFalse)
    p=TextFonts;
  if (XDisplayHeight(display,XDefaultScreen(display)) >= 748)
    p++;
  while (*p != (char *) NULL)
  {
    if (font_info != (XFontStruct *) NULL)
      break;
    font_info=XLoadQueryFont(display,(char *) *p);
    p++;
  }
  return(font_info);
}