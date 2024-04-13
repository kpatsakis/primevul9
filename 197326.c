MagickExport Image *XImportImage(const ImageInfo *image_info,
  XImportInfo *ximage_info,ExceptionInfo *exception)
{
  Colormap
    *colormaps;

  Display
    *display;

  Image
    *image;

  int
    number_colormaps,
    number_windows,
    x;

  RectangleInfo
    crop_info;

  Status
    status;

  Window
    *children,
    client,
    prior_target,
    root,
    target;

  XTextProperty
    window_name;

  /*
    Open X server connection.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  if (image_info->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",
      image_info->filename);
  assert(ximage_info != (XImportInfo *) NULL);
  display=XOpenDisplay(image_info->server_name);
  if (display == (Display *) NULL)
    {
      ThrowXWindowException(XServerError,"UnableToOpenXServer",
        XDisplayName(image_info->server_name));
      return((Image *) NULL);
    }
  /*
    Set our forgiving exception handler.
  */
  (void) XSetErrorHandler(XError);
  /*
    Select target window.
  */
  crop_info.x=0;
  crop_info.y=0;
  crop_info.width=0;
  crop_info.height=0;
  root=XRootWindow(display,XDefaultScreen(display));
  target=(Window) NULL;
  if (*image_info->filename != '\0')
    {
      if (LocaleCompare(image_info->filename,"root") == 0)
        target=root;
      else
        {
          /*
            Select window by ID or name.
          */
          if (isdigit((int) ((unsigned char) *image_info->filename)) != 0)
            target=XWindowByID(display,root,(Window)
              strtol(image_info->filename,(char **) NULL,0));
          if (target == (Window) NULL)
            target=XWindowByName(display,root,image_info->filename);
          if (target == (Window) NULL)
            ThrowXWindowException(XServerError,"NoWindowWithSpecifiedIDExists",
              image_info->filename);
        }
    }
  /*
    If target window is not defined, interactively select one.
  */
  prior_target=target;
  if (target == (Window) NULL)
    target=XSelectWindow(display,&crop_info);
  if (target == (Window) NULL)
    ThrowXWindowException(XServerError,"UnableToReadXWindowImage",
      image_info->filename);
  client=target;   /* obsolete */
  if (target != root)
    {
      unsigned int
        d;

      status=XGetGeometry(display,target,&root,&x,&x,&d,&d,&d,&d);
      if (status != False)
        {
          for ( ; ; )
          {
            Window
              parent;

            /*
              Find window manager frame.
            */
            status=XQueryTree(display,target,&root,&parent,&children,&d);
            if ((status != False) && (children != (Window *) NULL))
              (void) XFree((char *) children);
            if ((status == False) || (parent == (Window) NULL) ||
                (parent == root))
              break;
            target=parent;
          }
          /*
            Get client window.
          */
          client=XClientWindow(display,target);
          if (ximage_info->frame == MagickFalse)
            target=client;
          if ((ximage_info->frame == MagickFalse) &&
              (prior_target != MagickFalse))
            target=prior_target;
        }
    }
  if (ximage_info->screen)
    {
      int
        y;

      Window
        child;

      XWindowAttributes
        window_attributes;

      /*
        Obtain window image directly from screen.
      */
      status=XGetWindowAttributes(display,target,&window_attributes);
      if (status == False)
        {
          ThrowXWindowException(XServerError,"UnableToReadXWindowAttributes",
            image_info->filename);
          (void) XCloseDisplay(display);
          return((Image *) NULL);
        }
      (void) XTranslateCoordinates(display,target,root,0,0,&x,&y,&child);
      crop_info.x=(ssize_t) x;
      crop_info.y=(ssize_t) y;
      crop_info.width=(size_t) window_attributes.width;
      crop_info.height=(size_t) window_attributes.height;
      if (ximage_info->borders != 0)
        {
          /*
            Include border in image.
          */
          crop_info.x-=window_attributes.border_width;
          crop_info.y-=window_attributes.border_width;
          crop_info.width+=window_attributes.border_width << 1;
          crop_info.height+=window_attributes.border_width << 1;
        }
      target=root;
    }
  /*
    If WM_COLORMAP_WINDOWS property is set or multiple colormaps, descend.
  */
  number_windows=0;
  status=XGetWMColormapWindows(display,target,&children,&number_windows);
  if ((status == True) && (number_windows > 0))
    {
      ximage_info->descend=MagickTrue;
      (void) XFree ((char *) children);
    }
  colormaps=XListInstalledColormaps(display,target,&number_colormaps);
  if (number_colormaps > 0)
    {
      if (number_colormaps > 1)
        ximage_info->descend=MagickTrue;
      (void) XFree((char *) colormaps);
    }
  /*
    Alert the user not to alter the screen.
  */
  if (ximage_info->silent == MagickFalse)
    (void) XBell(display,0);
  /*
    Get image by window id.
  */
  (void) XGrabServer(display);
  image=XGetWindowImage(display,target,ximage_info->borders,
    ximage_info->descend ? 1U : 0U,exception);
  (void) XUngrabServer(display);
  if (image == (Image *) NULL)
    ThrowXWindowException(XServerError,"UnableToReadXWindowImage",
      image_info->filename)
  else
    {
      (void) CopyMagickString(image->filename,image_info->filename,
        MagickPathExtent);
      if ((crop_info.width != 0) && (crop_info.height != 0))
        {
          Image
            *clone_image,
            *crop_image;

          /*
            Crop image as defined by the cropping rectangle.
          */
          clone_image=CloneImage(image,0,0,MagickTrue,exception);
          if (clone_image != (Image *) NULL)
            {
              crop_image=CropImage(clone_image,&crop_info,exception);
              if (crop_image != (Image *) NULL)
                {
                  image=DestroyImage(image);
                  image=crop_image;
                }
            }
        }
      status=XGetWMName(display,target,&window_name);
      if (status == True)
        {
          if (*image_info->filename == '\0')
            (void) CopyMagickString(image->filename,(char *) window_name.value,
              (size_t) window_name.nitems+1);
          (void) XFree((void *) window_name.value);
        }
    }
  if (ximage_info->silent == MagickFalse)
    {
      /*
        Alert the user we're done.
      */
      (void) XBell(display,0);
      (void) XBell(display,0);
    }
  (void) XCloseDisplay(display);
  return(image);
}