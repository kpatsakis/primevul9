MagickExport void XAnimateBackgroundImage(Display *display,
  XResourceInfo *resource_info,Image *images)
{
  char
    geometry[MaxTextExtent],
    visual_type[MaxTextExtent];

  Image
    *coalesce_image,
    *display_image,
    **image_list;

  int
    scene;

  MagickStatusType
    status;

  RectangleInfo
    geometry_info;

  register ssize_t
    i;

  size_t
    delay,
    number_scenes;

  ssize_t
    iterations;

  static XPixelInfo
    pixel;

  static XStandardColormap
    *map_info;

  static XVisualInfo
    *visual_info = (XVisualInfo *) NULL;

  static XWindowInfo
    window_info;

  unsigned int
    height,
    width;

  Window
    root_window;

  XEvent
    event;

  XGCValues
    context_values;

  XResourceInfo
    resources;

  XWindowAttributes
    window_attributes;

  /*
    Determine target window.
  */
  assert(images != (Image *) NULL);
  assert(images->signature == MagickCoreSignature);
  if (images->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",images->filename);
  resources=(*resource_info);
  window_info.id=(Window) NULL;
  root_window=XRootWindow(display,XDefaultScreen(display));
  if (LocaleCompare(resources.window_id,"root") == 0)
    window_info.id=root_window;
  else
    {
      if (isdigit((int) ((unsigned char) *resources.window_id)) != 0)
        window_info.id=XWindowByID(display,root_window,
          (Window) strtol((char *) resources.window_id,(char **) NULL,0));
      if (window_info.id == (Window) NULL)
        window_info.id=
          XWindowByName(display,root_window,resources.window_id);
    }
  if (window_info.id == (Window) NULL)
    {
      ThrowXWindowException(XServerError,"NoWindowWithSpecifiedIDExists",
        resources.window_id);
      return;
    }
  /*
    Determine window visual id.
  */
  window_attributes.width=XDisplayWidth(display,XDefaultScreen(display));
  window_attributes.height=XDisplayHeight(display,XDefaultScreen(display));
  (void) CopyMagickString(visual_type,"default",MaxTextExtent);
  status=XGetWindowAttributes(display,window_info.id,&window_attributes) != 0 ?
    MagickTrue : MagickFalse;
  if (status != MagickFalse)
    (void) FormatLocaleString(visual_type,MaxTextExtent,"0x%lx",
      XVisualIDFromVisual(window_attributes.visual));
  if (visual_info == (XVisualInfo *) NULL)
    {
      /*
        Allocate standard colormap.
      */
      map_info=XAllocStandardColormap();
      if (map_info == (XStandardColormap *) NULL)
        ThrowXWindowFatalException(ResourceLimitFatalError,
          "MemoryAllocationFailed",images->filename);
      map_info->colormap=(Colormap) NULL;
      pixel.pixels=(unsigned long *) NULL;
      /*
        Initialize visual info.
      */
      resources.map_type=(char *) NULL;
      resources.visual_type=visual_type;
      visual_info=XBestVisualInfo(display,map_info,&resources);
      if (visual_info == (XVisualInfo *) NULL)
        ThrowXWindowFatalException(XServerFatalError,"UnableToGetVisual",
          images->filename);
      /*
        Initialize window info.
      */
      window_info.ximage=(XImage *) NULL;
      window_info.matte_image=(XImage *) NULL;
      window_info.pixmap=(Pixmap) NULL;
      window_info.matte_pixmap=(Pixmap) NULL;
    }
  /*
    Free previous root colors.
  */
  if (window_info.id == root_window)
    XDestroyWindowColors(display,root_window);
  coalesce_image=CoalesceImages(images,&images->exception);
  if (coalesce_image == (Image *) NULL)
    ThrowXWindowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed",
      images->filename);
  images=coalesce_image;
  if (resources.map_type == (char *) NULL)
    if ((visual_info->klass != TrueColor) &&
        (visual_info->klass != DirectColor))
      {
        Image
          *next;

        /*
          Determine if the sequence of images has the identical colormap.
        */
        for (next=images; next != (Image *) NULL; )
        {
          next->matte=MagickFalse;
          if ((next->storage_class == DirectClass) ||
              (next->colors != images->colors) ||
              (next->colors > (size_t) visual_info->colormap_size))
            break;
          for (i=0; i < (ssize_t) images->colors; i++)
            if (IsColorEqual(next->colormap+i,images->colormap+i) == MagickFalse)
              break;
          if (i < (ssize_t) images->colors)
            break;
          next=GetNextImageInList(next);
        }
        if (next != (Image *) NULL)
          (void) RemapImages(resources.quantize_info,images,(Image *) NULL);
      }
  /*
    Sort images by increasing scene number.
  */
  number_scenes=GetImageListLength(images);
  image_list=ImageListToArray(images,&images->exception);
  if (image_list == (Image **) NULL)
    ThrowXWindowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed",
      images->filename);
  for (i=0; i < (ssize_t) number_scenes; i++)
    if (image_list[i]->scene == 0)
      break;
  if (i == (ssize_t) number_scenes)
    qsort((void *) image_list,number_scenes,sizeof(Image *),SceneCompare);
  /*
    Initialize Standard Colormap.
  */
  resources.colormap=SharedColormap;
  display_image=image_list[0];
  for (scene=0; scene < (int) number_scenes; scene++)
  {
    if ((resource_info->map_type != (char *) NULL) ||
        (visual_info->klass == TrueColor) ||
        (visual_info->klass == DirectColor))
      (void) SetImageType(image_list[scene],image_list[scene]->matte ==
        MagickFalse ? TrueColorType : TrueColorMatteType);
    if ((display_image->columns < image_list[scene]->columns) &&
        (display_image->rows < image_list[scene]->rows))
      display_image=image_list[scene];
  }
  if ((resource_info->map_type != (char *) NULL) ||
      (visual_info->klass == TrueColor) || (visual_info->klass == DirectColor))
    (void) SetImageType(display_image,display_image->matte == MagickFalse ?
      TrueColorType : TrueColorMatteType);
  XMakeStandardColormap(display,visual_info,&resources,display_image,map_info,
    &pixel);
  /*
    Graphic context superclass.
  */
  context_values.background=pixel.background_color.pixel;
  context_values.foreground=pixel.foreground_color.pixel;
  pixel.annotate_context=XCreateGC(display,window_info.id,(unsigned long)
    (GCBackground | GCForeground),&context_values);
  if (pixel.annotate_context == (GC) NULL)
    ThrowXWindowFatalException(XServerFatalError,"UnableToCreateGraphicContext",
      images->filename);
  /*
    Initialize Image window attributes.
  */
  XGetWindowInfo(display,visual_info,map_info,&pixel,(XFontStruct *) NULL,
    &resources,&window_info);
  /*
    Create the X image.
  */
  window_info.width=(unsigned int) image_list[0]->columns;
  window_info.height=(unsigned int) image_list[0]->rows;
  if ((image_list[0]->columns != window_info.width) ||
      (image_list[0]->rows != window_info.height))
    ThrowXWindowFatalException(XServerFatalError,"UnableToCreateXImage",
      image_list[0]->filename);
  (void) FormatLocaleString(geometry,MaxTextExtent,"%ux%u+0+0>",
    window_attributes.width,window_attributes.height);
  geometry_info.width=window_info.width;
  geometry_info.height=window_info.height;
  geometry_info.x=(ssize_t) window_info.x;
  geometry_info.y=(ssize_t) window_info.y;
  (void) ParseMetaGeometry(geometry,&geometry_info.x,&geometry_info.y,
    &geometry_info.width,&geometry_info.height);
  window_info.width=(unsigned int) geometry_info.width;
  window_info.height=(unsigned int) geometry_info.height;
  window_info.x=(int) geometry_info.x;
  window_info.y=(int) geometry_info.y;
  status=XMakeImage(display,&resources,&window_info,image_list[0],
    window_info.width,window_info.height);
  if (status == MagickFalse)
    ThrowXWindowFatalException(XServerFatalError,"UnableToCreateXImage",
      images->filename);
  window_info.x=0;
  window_info.y=0;
  if (display_image->debug != MagickFalse)
    {
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "Image: %s[%.20g] %.20gx%.20g ",image_list[0]->filename,(double)
        image_list[0]->scene,(double) image_list[0]->columns,(double)
        image_list[0]->rows);
      if (image_list[0]->colors != 0)
        (void) LogMagickEvent(X11Event,GetMagickModule(),"%.20gc ",(double)
          image_list[0]->colors);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"%s",
        image_list[0]->magick);
    }
  /*
    Adjust image dimensions as specified by backdrop or geometry options.
  */
  width=window_info.width;
  height=window_info.height;
  if (resources.backdrop != MagickFalse)
    {
      /*
        Center image on window.
      */
      window_info.x=(int) (window_attributes.width/2)-
        (window_info.ximage->width/2);
      window_info.y=(int) (window_attributes.height/2)-
        (window_info.ximage->height/2);
      width=(unsigned int) window_attributes.width;
      height=(unsigned int) window_attributes.height;
    }
  if (resources.image_geometry != (char *) NULL)
    {
      char
        default_geometry[MaxTextExtent];

      int
        flags,
        gravity;

      XSizeHints
        *size_hints;

      /*
        User specified geometry.
      */
      size_hints=XAllocSizeHints();
      if (size_hints == (XSizeHints *) NULL)
        ThrowXWindowFatalException(ResourceLimitFatalError,
          "MemoryAllocationFailed",images->filename);
      size_hints->flags=0L;
      (void) FormatLocaleString(default_geometry,MaxTextExtent,"%ux%u",width,
        height);
      flags=XWMGeometry(display,visual_info->screen,resources.image_geometry,
        default_geometry,window_info.border_width,size_hints,&window_info.x,
        &window_info.y,(int *) &width,(int *) &height,&gravity);
      if (((flags & (XValue | YValue))) != 0)
        {
          width=(unsigned int) window_attributes.width;
          height=(unsigned int) window_attributes.height;
        }
      (void) XFree((void *) size_hints);
    }
  /*
    Create the X pixmap.
  */
  window_info.pixmap=XCreatePixmap(display,window_info.id,(unsigned int) width,
    (unsigned int) height,window_info.depth);
  if (window_info.pixmap == (Pixmap) NULL)
    ThrowXWindowFatalException(XServerFatalError,"UnableToCreateXPixmap",
      images->filename);
  /*
    Display pixmap on the window.
  */
  if (((unsigned int) width > window_info.width) ||
      ((unsigned int) height > window_info.height))
    (void) XFillRectangle(display,window_info.pixmap,
      window_info.annotate_context,0,0,(unsigned int) width,
      (unsigned int) height);
  (void) XPutImage(display,window_info.pixmap,window_info.annotate_context,
    window_info.ximage,0,0,window_info.x,window_info.y,window_info.width,
    window_info.height);
  (void) XSetWindowBackgroundPixmap(display,window_info.id,window_info.pixmap);
  (void) XClearWindow(display,window_info.id);
  /*
    Initialize image pixmaps structure.
  */
  window_info.pixmaps=(Pixmap *) AcquireQuantumMemory(number_scenes,
    sizeof(*window_info.pixmaps));
  window_info.matte_pixmaps=(Pixmap *) AcquireQuantumMemory(number_scenes,
    sizeof(*window_info.matte_pixmaps));
  if ((window_info.pixmaps == (Pixmap *) NULL) ||
      (window_info.matte_pixmaps == (Pixmap *) NULL))
    ThrowXWindowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed",
      images->filename);
  window_info.pixmaps[0]=window_info.pixmap;
  window_info.matte_pixmaps[0]=window_info.pixmap;
  for (scene=1; scene < (int) number_scenes; scene++)
  {
    unsigned int
      columns,
      rows;

    /*
      Create X image.
    */
    window_info.pixmap=(Pixmap) NULL;
    window_info.matte_pixmap=(Pixmap) NULL;
    if ((resources.map_type != (char *) NULL) ||
        (visual_info->klass == TrueColor) ||
        (visual_info->klass == DirectColor))
      if (image_list[scene]->storage_class == PseudoClass)
        XGetPixelPacket(display,visual_info,map_info,&resources,
          image_list[scene],window_info.pixel_info);
    columns=(unsigned int) image_list[scene]->columns;
    rows=(unsigned int) image_list[scene]->rows;
    if ((image_list[scene]->columns != columns) ||
        (image_list[scene]->rows != rows))
      ThrowXWindowFatalException(XServerFatalError,"UnableToCreateXImage",
        image_list[scene]->filename);
    status=XMakeImage(display,&resources,&window_info,image_list[scene],
      columns,rows);
    if (status == MagickFalse)
      ThrowXWindowFatalException(XServerFatalError,"UnableToCreateXImage",
        images->filename);
    if (display_image->debug != MagickFalse)
      {
        (void) LogMagickEvent(X11Event,GetMagickModule(),
          "Image: [%.20g] %s %.20gx%.20g ",(double) image_list[scene]->scene,
          image_list[scene]->filename,(double) columns,(double) rows);
        if (image_list[scene]->colors != 0)
          (void) LogMagickEvent(X11Event,GetMagickModule(),"%.20gc ",(double)
            image_list[scene]->colors);
        (void) LogMagickEvent(X11Event,GetMagickModule(),"%s",
          image_list[scene]->magick);
      }
    /*
      Create the X pixmap.
    */
    window_info.pixmap=XCreatePixmap(display,window_info.id,width,height,
      window_info.depth);
    if (window_info.pixmap == (Pixmap) NULL)
      ThrowXWindowFatalException(XServerFatalError,"UnableToCreateXPixmap",
        images->filename);
    /*
      Display pixmap on the window.
    */
    if ((width > window_info.width) || (height > window_info.height))
      (void) XFillRectangle(display,window_info.pixmap,
        window_info.annotate_context,0,0,width,height);
    (void) XPutImage(display,window_info.pixmap,window_info.annotate_context,
      window_info.ximage,0,0,window_info.x,window_info.y,window_info.width,
      window_info.height);
    (void) XSetWindowBackgroundPixmap(display,window_info.id,
      window_info.pixmap);
    (void) XClearWindow(display,window_info.id);
    window_info.pixmaps[scene]=window_info.pixmap;
    window_info.matte_pixmaps[scene]=window_info.matte_pixmap;
    if (image_list[scene]->matte)
      (void) XClearWindow(display,window_info.id);
    delay=1000*image_list[scene]->delay/MagickMax(
      image_list[scene]->ticks_per_second,1L);
    XDelay(display,resources.delay*(delay == 0 ? 10 : delay));
  }
  window_info.pixel_info=(&pixel);
  /*
    Display pixmap on the window.
  */
  (void) XSelectInput(display,window_info.id,SubstructureNotifyMask);
  event.type=Expose;
  iterations=0;
  do
  {
    for (scene=0; scene < (int) number_scenes; scene++)
    {
      if (XEventsQueued(display,QueuedAfterFlush) > 0)
        {
          (void) XNextEvent(display,&event);
          if (event.type == DestroyNotify)
            break;
        }
      window_info.pixmap=window_info.pixmaps[scene];
      window_info.matte_pixmap=window_info.matte_pixmaps[scene];
      (void) XSetWindowBackgroundPixmap(display,window_info.id,
        window_info.pixmap);
      (void) XClearWindow(display,window_info.id);
      (void) XSync(display,MagickFalse);
      delay=1000*image_list[scene]->delay/MagickMax(
        image_list[scene]->ticks_per_second,1L);
      XDelay(display,resources.delay*(delay == 0 ? 10 : delay));
    }
    iterations++;
    if (iterations == (ssize_t) image_list[0]->iterations)
      break;
  } while (event.type != DestroyNotify);
  (void) XSync(display,MagickFalse);
  image_list=(Image **) RelinquishMagickMemory(image_list);
  images=DestroyImageList(images);
}