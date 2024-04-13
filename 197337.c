MagickPrivate Cursor XMakeCursor(Display *display,Window window,
  Colormap colormap,char *background_color,char *foreground_color)
{
#define scope_height 17
#define scope_x_hot 8
#define scope_y_hot 8
#define scope_width 17

  static const unsigned char
    scope_bits[] =
    {
      0x80, 0x03, 0x00, 0x80, 0x02, 0x00, 0x80, 0x02, 0x00, 0x80, 0x02,
      0x00, 0x80, 0x02, 0x00, 0x80, 0x02, 0x00, 0x80, 0x02, 0x00, 0x7f,
      0xfc, 0x01, 0x01, 0x00, 0x01, 0x7f, 0xfc, 0x01, 0x80, 0x02, 0x00,
      0x80, 0x02, 0x00, 0x80, 0x02, 0x00, 0x80, 0x02, 0x00, 0x80, 0x02,
      0x00, 0x80, 0x02, 0x00, 0x80, 0x03, 0x00
    },
    scope_mask_bits[] =
    {
      0xc0, 0x07, 0x00, 0xc0, 0x07, 0x00, 0xc0, 0x06, 0x00, 0xc0, 0x06,
      0x00, 0xc0, 0x06, 0x00, 0xc0, 0x06, 0x00, 0xff, 0xfe, 0x01, 0x7f,
      0xfc, 0x01, 0x03, 0x80, 0x01, 0x7f, 0xfc, 0x01, 0xff, 0xfe, 0x01,
      0xc0, 0x06, 0x00, 0xc0, 0x06, 0x00, 0xc0, 0x06, 0x00, 0xc0, 0x06,
      0x00, 0xc0, 0x07, 0x00, 0xc0, 0x07, 0x00
    };

  Cursor
    cursor;

  Pixmap
    mask,
    source;

  XColor
    background,
    foreground;

  assert(display != (Display *) NULL);
  assert(window != (Window) NULL);
  assert(colormap != (Colormap) NULL);
  assert(background_color != (char *) NULL);
  assert(foreground_color != (char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",background_color);
  source=XCreateBitmapFromData(display,window,(char *) scope_bits,scope_width,
    scope_height);
  mask=XCreateBitmapFromData(display,window,(char *) scope_mask_bits,
    scope_width,scope_height);
  if ((source == (Pixmap) NULL) || (mask == (Pixmap) NULL))
    {
      ThrowXWindowException(XServerError,"UnableToCreatePixmap","...");
      return((Cursor) NULL);
    }
  (void) XParseColor(display,colormap,background_color,&background);
  (void) XParseColor(display,colormap,foreground_color,&foreground);
  cursor=XCreatePixmapCursor(display,source,mask,&foreground,&background,
    scope_x_hot,scope_y_hot);
  (void) XFreePixmap(display,source);
  (void) XFreePixmap(display,mask);
  return(cursor);
}