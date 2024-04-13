MagickExport MagickBooleanType XDrawImage(Display *display,
  const XPixelInfo *pixel,XDrawInfo *draw_info,Image *image)
{
  CacheView
    *draw_view;

  ExceptionInfo
    *exception;

  GC
    draw_context;

  Image
    *draw_image;

  int
    x,
    y;

  MagickBooleanType
    matte;

  Pixmap
    draw_pixmap;

  unsigned int
    depth,
    height,
    width;

  Window
    root_window;

  XGCValues
    context_values;

  XImage
    *draw_ximage;

  /*
    Initialize drawd image.
  */
  assert(display != (Display *) NULL);
  assert(pixel != (XPixelInfo *) NULL);
  assert(draw_info != (XDrawInfo *) NULL);
  assert(image != (Image *) NULL);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  /*
    Initialize drawd pixmap.
  */
  root_window=XRootWindow(display,XDefaultScreen(display));
  depth=(unsigned int) XDefaultDepth(display,XDefaultScreen(display));
  draw_pixmap=XCreatePixmap(display,root_window,draw_info->width,
    draw_info->height,depth);
  if (draw_pixmap == (Pixmap) NULL)
    return(MagickFalse);
  /*
    Initialize graphics info.
  */
  context_values.background=(size_t) (~0);
  context_values.foreground=0;
  context_values.line_width=(int) draw_info->line_width;
  draw_context=XCreateGC(display,root_window,(size_t)
    (GCBackground | GCForeground | GCLineWidth),&context_values);
  if (draw_context == (GC) NULL)
    return(MagickFalse);
  /*
    Clear pixmap.
  */
  (void) XFillRectangle(display,draw_pixmap,draw_context,0,0,draw_info->width,
    draw_info->height);
  /*
    Draw line to pixmap.
  */
  (void) XSetBackground(display,draw_context,0);
  (void) XSetForeground(display,draw_context,(size_t) (~0));
  if (draw_info->stipple !=  (Pixmap) NULL)
    {
      (void) XSetFillStyle(display,draw_context,FillOpaqueStippled);
      (void) XSetStipple(display,draw_context,draw_info->stipple);
    }
  switch (draw_info->element)
  {
    case PointElement:
    default:
    {
      (void) XDrawLines(display,draw_pixmap,draw_context,
        draw_info->coordinate_info,(int) draw_info->number_coordinates,
        CoordModeOrigin);
      break;
    }
    case LineElement:
    {
      (void) XDrawLine(display,draw_pixmap,draw_context,draw_info->line_info.x1,
        draw_info->line_info.y1,draw_info->line_info.x2,
        draw_info->line_info.y2);
      break;
    }
    case RectangleElement:
    {
      (void) XDrawRectangle(display,draw_pixmap,draw_context,
        (int) draw_info->rectangle_info.x,(int) draw_info->rectangle_info.y,
        (unsigned int) draw_info->rectangle_info.width,
        (unsigned int) draw_info->rectangle_info.height);
      break;
    }
    case FillRectangleElement:
    {
      (void) XFillRectangle(display,draw_pixmap,draw_context,
        (int) draw_info->rectangle_info.x,(int) draw_info->rectangle_info.y,
        (unsigned int) draw_info->rectangle_info.width,
        (unsigned int) draw_info->rectangle_info.height);
      break;
    }
    case CircleElement:
    case EllipseElement:
    {
      (void) XDrawArc(display,draw_pixmap,draw_context,
        (int) draw_info->rectangle_info.x,(int) draw_info->rectangle_info.y,
        (unsigned int) draw_info->rectangle_info.width,
        (unsigned int) draw_info->rectangle_info.height,0,360*64);
      break;
    }
    case FillCircleElement:
    case FillEllipseElement:
    {
      (void) XFillArc(display,draw_pixmap,draw_context,
        (int) draw_info->rectangle_info.x,(int) draw_info->rectangle_info.y,
        (unsigned int) draw_info->rectangle_info.width,
        (unsigned int) draw_info->rectangle_info.height,0,360*64);
      break;
    }
    case PolygonElement:
    {
      XPoint
        *coordinate_info;

      coordinate_info=draw_info->coordinate_info;
      (void) XDrawLines(display,draw_pixmap,draw_context,coordinate_info,
        (int) draw_info->number_coordinates,CoordModeOrigin);
      (void) XDrawLine(display,draw_pixmap,draw_context,
        coordinate_info[draw_info->number_coordinates-1].x,
        coordinate_info[draw_info->number_coordinates-1].y,
        coordinate_info[0].x,coordinate_info[0].y);
      break;
    }
    case FillPolygonElement:
    {
      (void) XFillPolygon(display,draw_pixmap,draw_context,
        draw_info->coordinate_info,(int) draw_info->number_coordinates,Complex,
        CoordModeOrigin);
      break;
    }
  }
  (void) XFreeGC(display,draw_context);
  /*
    Initialize X image.
  */
  draw_ximage=XGetImage(display,draw_pixmap,0,0,draw_info->width,
    draw_info->height,AllPlanes,ZPixmap);
  if (draw_ximage == (XImage *) NULL)
    return(MagickFalse);
  (void) XFreePixmap(display,draw_pixmap);
  /*
    Initialize draw image.
  */
  draw_image=AcquireImage((ImageInfo *) NULL);
  if (draw_image == (Image *) NULL)
    return(MagickFalse);
  draw_image->columns=draw_info->width;
  draw_image->rows=draw_info->height;
  /*
    Transfer drawn X image to image.
  */
  width=(unsigned int) image->columns;
  height=(unsigned int) image->rows;
  x=0;
  y=0;
  (void) XParseGeometry(draw_info->geometry,&x,&y,&width,&height);
  (void) GetOneVirtualPixel(image,(ssize_t) x,(ssize_t) y,
    &draw_image->background_color,&image->exception);
  if (SetImageStorageClass(draw_image,DirectClass) == MagickFalse)
    return(MagickFalse);
  draw_image->matte=MagickTrue;
  exception=(&image->exception);
  draw_view=AcquireAuthenticCacheView(draw_image,exception);
  for (y=0; y < (int) draw_image->rows; y++)
  {
    register int
      x;

    register PixelPacket
      *magick_restrict q;

    q=QueueCacheViewAuthenticPixels(draw_view,0,(ssize_t) y,draw_image->columns,
      1,exception);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) draw_image->columns; x++)
    {
      if (XGetPixel(draw_ximage,x,y) == 0)
        {
          /*
            Set this pixel to the background color.
          */
          *q=draw_image->background_color;
          q->opacity=(Quantum) (draw_info->stencil == OpaqueStencil ?
            TransparentOpacity : OpaqueOpacity);
        }
      else
        {
          /*
            Set this pixel to the pen color.
          */
          SetPixelRed(q,ScaleShortToQuantum(pixel->pen_color.red));
          SetPixelGreen(q,ScaleShortToQuantum(pixel->pen_color.green));
          SetPixelBlue(q,ScaleShortToQuantum(pixel->pen_color.blue));
          SetPixelOpacity(q,(Quantum) (draw_info->stencil ==
            OpaqueStencil ? OpaqueOpacity : TransparentOpacity));
        }
      q++;
    }
    if (SyncCacheViewAuthenticPixels(draw_view,exception) == MagickFalse)
      break;
  }
  draw_view=DestroyCacheView(draw_view);
  XDestroyImage(draw_ximage);
  /*
    Determine draw geometry.
  */
  (void) XParseGeometry(draw_info->geometry,&x,&y,&width,&height);
  if ((width != (unsigned int) draw_image->columns) ||
      (height != (unsigned int) draw_image->rows))
    {
      char
        image_geometry[MaxTextExtent];

      /*
        Scale image.
      */
      (void) FormatLocaleString(image_geometry,MaxTextExtent,"%ux%u",
        width,height);
      (void) TransformImage(&draw_image,(char *) NULL,image_geometry);
    }
  if (draw_info->degrees != 0.0)
    {
      Image
        *rotate_image;

      int
        rotations;

      MagickRealType
        normalized_degrees;

      /*
        Rotate image.
      */
      rotate_image=RotateImage(draw_image,draw_info->degrees,&image->exception);
      if (rotate_image == (Image *) NULL)
        return(MagickFalse);
      draw_image=DestroyImage(draw_image);
      draw_image=rotate_image;
      /*
        Annotation is relative to the degree of rotation.
      */
      normalized_degrees=draw_info->degrees;
      while (normalized_degrees < -45.0)
        normalized_degrees+=360.0;
      for (rotations=0; normalized_degrees > 45.0; rotations++)
        normalized_degrees-=90.0;
      switch (rotations % 4)
      {
        default:
        case 0:
          break;
        case 1:
        {
          /*
            Rotate 90 degrees.
          */
          x=x-(int) draw_image->columns/2;
          y=y+(int) draw_image->columns/2;
          break;
        }
        case 2:
        {
          /*
            Rotate 180 degrees.
          */
          x=x-(int) draw_image->columns;
          break;
        }
        case 3:
        {
          /*
            Rotate 270 degrees.
          */
          x=x-(int) draw_image->columns/2;
          y=y-(int) (draw_image->rows-(draw_image->columns/2));
          break;
        }
      }
    }
  /*
    Composite text onto the image.
  */
  draw_view=AcquireAuthenticCacheView(draw_image,exception);
  for (y=0; y < (int) draw_image->rows; y++)
  {
    register int
      x;

    register PixelPacket
      *magick_restrict q;

    q=GetCacheViewAuthenticPixels(draw_view,0,(ssize_t) y,draw_image->columns,1,
      exception);
    if (q == (PixelPacket *) NULL)
      break;
    for (x=0; x < (int) draw_image->columns; x++)
    {
      if (q->opacity != (Quantum) TransparentOpacity)
        SetPixelOpacity(q,OpaqueOpacity);
      q++;
    }
    if (SyncCacheViewAuthenticPixels(draw_view,exception) == MagickFalse)
      break;
  }
  draw_view=DestroyCacheView(draw_view);
  (void) XParseGeometry(draw_info->geometry,&x,&y,&width,&height);
  if (draw_info->stencil == TransparentStencil)
    (void) CompositeImage(image,CopyOpacityCompositeOp,draw_image,(ssize_t) x,
      (ssize_t) y);
  else
    {
      matte=image->matte;
      (void) CompositeImage(image,OverCompositeOp,draw_image,(ssize_t) x,
        (ssize_t) y);
      image->matte=matte;
    }
  draw_image=DestroyImage(draw_image);
  return(MagickTrue);
}