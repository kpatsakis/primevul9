MagickExport void XMakeMagnifyImage(Display *display,XWindows *windows)
{
  char
    tuple[MaxTextExtent];

  int
    y;

  MagickPixelPacket
    pixel;

  register int
    x;

  register ssize_t
    i;

  register unsigned char
    *p,
    *q;

  ssize_t
    n;

  static unsigned int
    previous_magnify = 0;

  static XWindowInfo
    magnify_window;

  unsigned int
    height,
    j,
    k,
    l,
    magnify,
    scanline_pad,
    width;

  XImage
    *ximage;

  /*
    Check boundary conditions.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(windows != (XWindows *) NULL);
  magnify=1;
  for (n=1; n < (ssize_t) windows->magnify.data; n++)
    magnify<<=1;
  while ((magnify*windows->image.ximage->width) < windows->magnify.width)
    magnify<<=1;
  while ((magnify*windows->image.ximage->height) < windows->magnify.height)
    magnify<<=1;
  while (magnify > windows->magnify.width)
    magnify>>=1;
  while (magnify > windows->magnify.height)
    magnify>>=1;
  if (magnify == 0)
    magnify=1;
  if (magnify != previous_magnify)
    {
      Status
        status;

      XTextProperty
        window_name;

      /*
        New magnify factor:  update magnify window name.
      */
      i=0;
      while ((1 << i) <= (int) magnify)
        i++;
      (void) FormatLocaleString(windows->magnify.name,MaxTextExtent,
        "Magnify %.20gX",(double) i);
      status=XStringListToTextProperty(&windows->magnify.name,1,&window_name);
      if (status != False)
        {
          XSetWMName(display,windows->magnify.id,&window_name);
          XSetWMIconName(display,windows->magnify.id,&window_name);
          (void) XFree((void *) window_name.value);
        }
    }
  previous_magnify=magnify;
  ximage=windows->image.ximage;
  width=(unsigned int) windows->magnify.ximage->width;
  height=(unsigned int) windows->magnify.ximage->height;
  if ((windows->magnify.x < 0) ||
      (windows->magnify.x >= windows->image.ximage->width))
    windows->magnify.x=windows->image.ximage->width >> 1;
  x=windows->magnify.x-((width/magnify) >> 1);
  if (x < 0)
    x=0;
  else
    if (x > (int) (ximage->width-(width/magnify)))
      x=ximage->width-width/magnify;
  if ((windows->magnify.y < 0) ||
      (windows->magnify.y >= windows->image.ximage->height))
    windows->magnify.y=windows->image.ximage->height >> 1;
  y=windows->magnify.y-((height/magnify) >> 1);
  if (y < 0)
    y=0;
  else
    if (y > (int) (ximage->height-(height/magnify)))
      y=ximage->height-height/magnify;
  q=(unsigned char *) windows->magnify.ximage->data;
  scanline_pad=(unsigned int) (windows->magnify.ximage->bytes_per_line-
    ((width*windows->magnify.ximage->bits_per_pixel) >> 3));
  if (ximage->bits_per_pixel < 8)
    {
      register unsigned char
        background,
        byte,
        foreground,
        p_bit,
        q_bit;

      register unsigned int
        plane;

      XPixelInfo
        *pixel_info;

      pixel_info=windows->magnify.pixel_info;
      switch (ximage->bitmap_bit_order)
      {
        case LSBFirst:
        {
          /*
            Magnify little-endian bitmap.
          */
          background=0x00;
          foreground=0x80;
          if (ximage->format == XYBitmap)
            {
              background=(unsigned char)
                (XPixelIntensity(&pixel_info->foreground_color) <
                 XPixelIntensity(&pixel_info->background_color) ?  0x80 : 0x00);
              foreground=(unsigned char)
                (XPixelIntensity(&pixel_info->background_color) <
                 XPixelIntensity(&pixel_info->foreground_color) ?  0x80 : 0x00);
              if (windows->magnify.depth > 1)
                Swap(background,foreground);
            }
          for (i=0; i < (ssize_t) height; i+=magnify)
          {
            /*
              Propogate pixel magnify rows.
            */
            for (j=0; j < magnify; j++)
            {
              p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
                ((x*ximage->bits_per_pixel) >> 3);
              p_bit=(unsigned char) (x*ximage->bits_per_pixel) & 0x07;
              q_bit=0;
              byte=0;
              for (k=0; k < width; k+=magnify)
              {
                /*
                  Propogate pixel magnify columns.
                */
                for (l=0; l < magnify; l++)
                {
                  /*
                    Propogate each bit plane.
                  */
                  for (plane=0; (int) plane < ximage->bits_per_pixel; plane++)
                  {
                    byte>>=1;
                    if (*p & (0x01 << (p_bit+plane)))
                      byte|=foreground;
                    else
                      byte|=background;
                    q_bit++;
                    if (q_bit == 8)
                      {
                        *q++=byte;
                        q_bit=0;
                        byte=0;
                      }
                  }
                }
                p_bit+=ximage->bits_per_pixel;
                if (p_bit == 8)
                  {
                    p++;
                    p_bit=0;
                  }
                if (q_bit != 0)
                  *q=byte >> (8-q_bit);
                q+=scanline_pad;
              }
            }
            y++;
          }
          break;
        }
        case MSBFirst:
        default:
        {
          /*
            Magnify big-endian bitmap.
          */
          background=0x00;
          foreground=0x01;
          if (ximage->format == XYBitmap)
            {
              background=(unsigned char)
                (XPixelIntensity(&pixel_info->foreground_color) <
                 XPixelIntensity(&pixel_info->background_color) ?  0x01 : 0x00);
              foreground=(unsigned char)
                (XPixelIntensity(&pixel_info->background_color) <
                 XPixelIntensity(&pixel_info->foreground_color) ?  0x01 : 0x00);
              if (windows->magnify.depth > 1)
                Swap(background,foreground);
            }
          for (i=0; i < (ssize_t) height; i+=magnify)
          {
            /*
              Propogate pixel magnify rows.
            */
            for (j=0; j < magnify; j++)
            {
              p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
                ((x*ximage->bits_per_pixel) >> 3);
              p_bit=(unsigned char) (x*ximage->bits_per_pixel) & 0x07;
              q_bit=0;
              byte=0;
              for (k=0; k < width; k+=magnify)
              {
                /*
                  Propogate pixel magnify columns.
                */
                for (l=0; l < magnify; l++)
                {
                  /*
                    Propogate each bit plane.
                  */
                  for (plane=0; (int) plane < ximage->bits_per_pixel; plane++)
                  {
                    byte<<=1;
                    if (*p & (0x80 >> (p_bit+plane)))
                      byte|=foreground;
                    else
                      byte|=background;
                    q_bit++;
                    if (q_bit == 8)
                      {
                        *q++=byte;
                        q_bit=0;
                        byte=0;
                      }
                  }
                }
                p_bit+=ximage->bits_per_pixel;
                if (p_bit == 8)
                  {
                    p++;
                    p_bit=0;
                  }
                if (q_bit != 0)
                  *q=byte << (8-q_bit);
                q+=scanline_pad;
              }
            }
            y++;
          }
          break;
        }
      }
    }
  else
    switch (ximage->bits_per_pixel)
    {
      case 6:
      case 8:
      {
        /*
          Magnify 8 bit X image.
        */
        for (i=0; i < (ssize_t) height; i+=magnify)
        {
          /*
            Propogate pixel magnify rows.
          */
          for (j=0; j < magnify; j++)
          {
            p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
              ((x*ximage->bits_per_pixel) >> 3);
            for (k=0; k < width; k+=magnify)
            {
              /*
                Propogate pixel magnify columns.
              */
              for (l=0; l < magnify; l++)
                *q++=(*p);
              p++;
            }
            q+=scanline_pad;
          }
          y++;
        }
        break;
      }
      default:
      {
        register unsigned int
          bytes_per_pixel,
          m;

        /*
          Magnify multi-byte X image.
        */
        bytes_per_pixel=(unsigned int) ximage->bits_per_pixel >> 3;
        for (i=0; i < (ssize_t) height; i+=magnify)
        {
          /*
            Propogate pixel magnify rows.
          */
          for (j=0; j < magnify; j++)
          {
            p=(unsigned char *) ximage->data+y*ximage->bytes_per_line+
              ((x*ximage->bits_per_pixel) >> 3);
            for (k=0; k < width; k+=magnify)
            {
              /*
                Propogate pixel magnify columns.
              */
              for (l=0; l < magnify; l++)
                for (m=0; m < bytes_per_pixel; m++)
                  *q++=(*(p+m));
              p+=bytes_per_pixel;
            }
            q+=scanline_pad;
          }
          y++;
        }
        break;
      }
    }
  /*
    Copy X image to magnify pixmap.
  */
  x=windows->magnify.x-((width/magnify) >> 1);
  if (x < 0)
    x=(int) ((width >> 1)-windows->magnify.x*magnify);
  else
    if (x > (int) (ximage->width-(width/magnify)))
      x=(int) ((ximage->width-windows->magnify.x)*magnify-(width >> 1));
    else
      x=0;
  y=windows->magnify.y-((height/magnify) >> 1);
  if (y < 0)
    y=(int) ((height >> 1)-windows->magnify.y*magnify);
  else
    if (y > (int) (ximage->height-(height/magnify)))
      y=(int) ((ximage->height-windows->magnify.y)*magnify-(height >> 1));
    else
      y=0;
  if ((x != 0) || (y != 0))
    (void) XFillRectangle(display,windows->magnify.pixmap,
      windows->magnify.annotate_context,0,0,width,height);
  (void) XPutImage(display,windows->magnify.pixmap,
    windows->magnify.annotate_context,windows->magnify.ximage,0,0,x,y,width-x,
    height-y);
  if ((magnify > 1) && ((magnify <= (width >> 1)) &&
      (magnify <= (height >> 1))))
    {
      RectangleInfo
        highlight_info;

      /*
        Highlight center pixel.
      */
      highlight_info.x=(ssize_t) windows->magnify.width >> 1;
      highlight_info.y=(ssize_t) windows->magnify.height >> 1;
      highlight_info.width=magnify;
      highlight_info.height=magnify;
      (void) XDrawRectangle(display,windows->magnify.pixmap,
        windows->magnify.highlight_context,(int) highlight_info.x,
        (int) highlight_info.y,(unsigned int) highlight_info.width-1,
        (unsigned int) highlight_info.height-1);
      if (magnify > 2)
        (void) XDrawRectangle(display,windows->magnify.pixmap,
          windows->magnify.annotate_context,(int) highlight_info.x+1,
          (int) highlight_info.y+1,(unsigned int) highlight_info.width-3,
          (unsigned int) highlight_info.height-3);
    }
  /*
    Show center pixel color.
  */
  (void) GetOneVirtualMagickPixel(windows->image.image,(ssize_t)
    windows->magnify.x,(ssize_t) windows->magnify.y,&pixel,
    &windows->image.image->exception);
  (void) FormatLocaleString(tuple,MaxTextExtent,"%d,%d: ",
    windows->magnify.x,windows->magnify.y);
  (void) ConcatenateMagickString(tuple,"(",MaxTextExtent);
  ConcatenateColorComponent(&pixel,RedChannel,X11Compliance,tuple);
  (void) ConcatenateMagickString(tuple,",",MaxTextExtent);
  ConcatenateColorComponent(&pixel,GreenChannel,X11Compliance,tuple);
  (void) ConcatenateMagickString(tuple,",",MaxTextExtent);
  ConcatenateColorComponent(&pixel,BlueChannel,X11Compliance,tuple);
  if (pixel.colorspace == CMYKColorspace)
    {
      (void) ConcatenateMagickString(tuple,",",MaxTextExtent);
      ConcatenateColorComponent(&pixel,IndexChannel,X11Compliance,tuple);
    }
  if (pixel.matte != MagickFalse)
    {
      (void) ConcatenateMagickString(tuple,",",MaxTextExtent);
      ConcatenateColorComponent(&pixel,OpacityChannel,X11Compliance,tuple);
    }
  (void) ConcatenateMagickString(tuple,")",MaxTextExtent);
  height=(unsigned int) windows->magnify.font_info->ascent+
    windows->magnify.font_info->descent;
  x=windows->magnify.font_info->max_bounds.width >> 1;
  y=windows->magnify.font_info->ascent+(height >> 2);
  (void) XDrawImageString(display,windows->magnify.pixmap,
    windows->magnify.annotate_context,x,y,tuple,(int) strlen(tuple));
  GetColorTuple(&pixel,MagickTrue,tuple);
  y+=height;
  (void) XDrawImageString(display,windows->magnify.pixmap,
    windows->magnify.annotate_context,x,y,tuple,(int) strlen(tuple));
  (void) QueryMagickColorname(windows->image.image,&pixel,SVGCompliance,tuple,
     &windows->image.image->exception);
  y+=height;
  (void) XDrawImageString(display,windows->magnify.pixmap,
    windows->magnify.annotate_context,x,y,tuple,(int) strlen(tuple));
  /*
    Refresh magnify window.
  */
  magnify_window=windows->magnify;
  magnify_window.x=0;
  magnify_window.y=0;
  XRefreshWindow(display,&magnify_window,(XEvent *) NULL);
}