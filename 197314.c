MagickPrivate MagickBooleanType XGetWindowColor(Display *display,
  XWindows *windows,char *name,ExceptionInfo *exception)
{
  int
    x,
    y;

  PixelInfo
    pixel;

  RectangleInfo
    crop_info;

  Status
    status;

  Window
    child,
    client_window,
    root_window,
    target_window;

  XColor
    color;

  XImage
    *ximage;

  XWindowAttributes
    window_attributes;

  /*
    Choose a pixel from the X server.
  */
  assert(display != (Display *) NULL);
  assert(name != (char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",name);
  *name='\0';
  target_window=XSelectWindow(display,&crop_info);
  if (target_window == (Window) NULL)
    return(MagickFalse);
  root_window=XRootWindow(display,XDefaultScreen(display));
  client_window=target_window;
  if (target_window != root_window)
    {
      unsigned int
        d;

      /*
        Get client window.
      */
      status=XGetGeometry(display,target_window,&root_window,&x,&x,&d,&d,&d,&d);
      if (status != False)
        {
          client_window=XClientWindow(display,target_window);
          target_window=client_window;
        }
    }
  /*
    Verify window is viewable.
  */
  status=XGetWindowAttributes(display,target_window,&window_attributes);
  if ((status == False) || (window_attributes.map_state != IsViewable))
    return(MagickFalse);
  /*
    Get window X image.
  */
  (void) XTranslateCoordinates(display,root_window,target_window,
    (int) crop_info.x,(int) crop_info.y,&x,&y,&child);
  ximage=XGetImage(display,target_window,x,y,1,1,AllPlanes,ZPixmap);
  if (ximage == (XImage *) NULL)
    return(MagickFalse);
  color.pixel=XGetPixel(ximage,0,0);
  XDestroyImage(ximage);
  /*
    Match color against the color database.
  */
  (void) XQueryColor(display,window_attributes.colormap,&color);
  pixel.red=(double) ScaleShortToQuantum(color.red);
  pixel.green=(double) ScaleShortToQuantum(color.green);
  pixel.blue=(double) ScaleShortToQuantum(color.blue);
  pixel.alpha=(MagickRealType) OpaqueAlpha;
  (void) QueryColorname(windows->image.image,&pixel,X11Compliance,name,
    exception);
  return(MagickTrue);
}