MagickPrivate MagickBooleanType XAnnotateImage(Display *display,
  const XPixelInfo *pixel,XAnnotateInfo *annotate_info,Image *image,
  ExceptionInfo *exception)
{
  CacheView
    *annotate_view;

  GC
    annotate_context;

  Image
    *annotate_image;

  int
    x,
    y;

  PixelTrait
    alpha_trait;

  Pixmap
    annotate_pixmap;

  unsigned int
    depth,
    height,
    width;

  Window
    root_window;

  XGCValues
    context_values;

  XImage
    *annotate_ximage;

  /*
    Initialize annotated image.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(pixel != (XPixelInfo *) NULL);
  assert(annotate_info != (XAnnotateInfo *) NULL);
  assert(image != (Image *) NULL);
  /*
    Initialize annotated pixmap.
  */
  root_window=XRootWindow(display,XDefaultScreen(display));
  depth=(unsigned int) XDefaultDepth(display,XDefaultScreen(display));
  annotate_pixmap=XCreatePixmap(display,root_window,annotate_info->width,
    annotate_info->height,depth);
  if (annotate_pixmap == (Pixmap) NULL)
    return(MagickFalse);
  /*
    Initialize graphics info.
  */
  context_values.background=0;
  context_values.foreground=(size_t) (~0);
  context_values.font=annotate_info->font_info->fid;
  annotate_context=XCreateGC(display,root_window,(unsigned long)
    (GCBackground | GCFont | GCForeground),&context_values);
  if (annotate_context == (GC) NULL)
    return(MagickFalse);
  /*
    Draw text to pixmap.
  */
  (void) XDrawImageString(display,annotate_pixmap,annotate_context,0,
    (int) annotate_info->font_info->ascent,annotate_info->text,
    (int) strlen(annotate_info->text));
  (void) XFreeGC(display,annotate_context);
  /*
    Initialize annotated X image.
  */
  annotate_ximage=XGetImage(display,annotate_pixmap,0,0,annotate_info->width,
    annotate_info->height,AllPlanes,ZPixmap);
  if (annotate_ximage == (XImage *) NULL)
    return(MagickFalse);
  (void) XFreePixmap(display,annotate_pixmap);
  /*
    Initialize annotated image.
  */
  annotate_image=AcquireImage((ImageInfo *) NULL,exception);
  if (annotate_image == (Image *) NULL)
    return(MagickFalse);
  annotate_image->columns=annotate_info->width;
  annotate_image->rows=annotate_info->height;
  /*
    Transfer annotated X image to image.
  */
  width=(unsigned int) image->columns;
  height=(unsigned int) image->rows;
  x=0;
  y=0;
  (void) XParseGeometry(annotate_info->geometry,&x,&y,&width,&height);
  (void) GetOneVirtualPixelInfo(image,UndefinedVirtualPixelMethod,(ssize_t) x,
    (ssize_t) y,&annotate_image->background_color,exception);
  if (annotate_info->stencil == ForegroundStencil)
    annotate_image->alpha_trait=BlendPixelTrait;
  annotate_view=AcquireAuthenticCacheView(annotate_image,exception);
  for (y=0; y < (int) annotate_image->rows; y++)
  {
    register int
      x;

    register Quantum
      *magick_restrict q;

    q=GetCacheViewAuthenticPixels(annotate_view,0,(ssize_t) y,
      annotate_image->columns,1,exception);
    if (q == (Quantum *) NULL)
      break;
    for (x=0; x < (int) annotate_image->columns; x++)
    {
      SetPixelAlpha(annotate_image,OpaqueAlpha,q);
      if (XGetPixel(annotate_ximage,x,y) == 0)
        {
          /*
            Set this pixel to the background color.
          */
          SetPixelRed(annotate_image,ScaleShortToQuantum(
            pixel->box_color.red),q);
          SetPixelGreen(annotate_image,ScaleShortToQuantum(
            pixel->box_color.green),q);
          SetPixelBlue(annotate_image,ScaleShortToQuantum(
            pixel->box_color.blue),q);
          if ((annotate_info->stencil == ForegroundStencil) ||
              (annotate_info->stencil == OpaqueStencil))
            SetPixelAlpha(annotate_image,TransparentAlpha,q);
        }
      else
        {
          /*
            Set this pixel to the pen color.
          */
          SetPixelRed(annotate_image,ScaleShortToQuantum(
            pixel->pen_color.red),q);
          SetPixelGreen(annotate_image,ScaleShortToQuantum(
            pixel->pen_color.green),q);
          SetPixelBlue(annotate_image,ScaleShortToQuantum(
            pixel->pen_color.blue),q);
          if (annotate_info->stencil == BackgroundStencil)
            SetPixelAlpha(annotate_image,TransparentAlpha,q);
        }
      q+=GetPixelChannels(annotate_image);
    }
    if (SyncCacheViewAuthenticPixels(annotate_view,exception) == MagickFalse)
      break;
  }
  annotate_view=DestroyCacheView(annotate_view);
  XDestroyImage(annotate_ximage);
  /*
    Determine annotate geometry.
  */
  (void) XParseGeometry(annotate_info->geometry,&x,&y,&width,&height);
  if ((width != (unsigned int) annotate_image->columns) ||
      (height != (unsigned int) annotate_image->rows))
    {
      char
        image_geometry[MagickPathExtent];

      /*
        Scale image.
      */
      (void) FormatLocaleString(image_geometry,MagickPathExtent,"%ux%u",
        width,height);
      (void) TransformImage(&annotate_image,(char *) NULL,image_geometry,
        exception);
    }
  if (annotate_info->degrees != 0.0)
    {
      Image
        *rotate_image;

      int
        rotations;

      double
        normalized_degrees;

      /*
        Rotate image.
      */
      rotate_image=RotateImage(annotate_image,annotate_info->degrees,exception);
      if (rotate_image == (Image *) NULL)
        return(MagickFalse);
      annotate_image=DestroyImage(annotate_image);
      annotate_image=rotate_image;
      /*
        Annotation is relative to the degree of rotation.
      */
      normalized_degrees=annotate_info->degrees;
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
          x-=(int) annotate_image->columns/2;
          y+=(int) annotate_image->columns/2;
          break;
        }
        case 2:
        {
          /*
            Rotate 180 degrees.
          */
          x=x-(int) annotate_image->columns;
          break;
        }
        case 3:
        {
          /*
            Rotate 270 degrees.
          */
          x=x-(int) annotate_image->columns/2;
          y=y-(int) (annotate_image->rows-(annotate_image->columns/2));
          break;
        }
      }
    }
  /*
    Composite text onto the image.
  */
  (void) XParseGeometry(annotate_info->geometry,&x,&y,&width,&height);
  alpha_trait=image->alpha_trait;
  (void) CompositeImage(image,annotate_image,
    annotate_image->alpha_trait != UndefinedPixelTrait ? OverCompositeOp :
    CopyCompositeOp,MagickTrue,(ssize_t) x,(ssize_t) y,exception);
  image->alpha_trait=alpha_trait;
  annotate_image=DestroyImage(annotate_image);
  return(MagickTrue);
}