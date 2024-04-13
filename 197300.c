MagickPrivate MagickBooleanType XQueryColorCompliance(const char *target,
  XColor *color)
{
  Colormap
    colormap;

  static Display
    *display = (Display *) NULL;

  Status
    status;

  XColor
    xcolor;

  /*
    Initialize color return value.
  */
  assert(color != (XColor *) NULL);
  color->red=0;
  color->green=0;
  color->blue=0;
  color->flags=(char) (DoRed | DoGreen | DoBlue);
  if ((target == (char *) NULL) || (*target == '\0'))
    target="#ffffffffffff";
  /*
    Let the X server define the color for us.
  */
  if (display == (Display *) NULL)
    display=XOpenDisplay((char *) NULL);
  if (display == (Display *) NULL)
    {
      ThrowXWindowException(XServerError,"ColorIsNotKnownToServer",target);
      return(MagickFalse);
    }
  colormap=XDefaultColormap(display,XDefaultScreen(display));
  status=XParseColor(display,colormap,(char *) target,&xcolor);
  if (status == False)
    ThrowXWindowException(XServerError,"ColorIsNotKnownToServer",target)
  else
    {
      color->red=xcolor.red;
      color->green=xcolor.green;
      color->blue=xcolor.blue;
      color->flags=xcolor.flags;
    }
  return(status != False ? MagickTrue : MagickFalse);
}