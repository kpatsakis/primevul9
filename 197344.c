MagickPrivate void XBestIconSize(Display *display,XWindowInfo *window,
  Image *image)
{
  int
    i,
    number_sizes;

  double
    scale_factor;

  unsigned int
    height,
    icon_height,
    icon_width,
    width;

  Window
    root_window;

  XIconSize
    *icon_size,
    *size_list;

  /*
    Determine if the window manager has specified preferred icon sizes.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(window != (XWindowInfo *) NULL);
  assert(image != (Image *) NULL);
  window->width=MaxIconSize;
  window->height=MaxIconSize;
  icon_size=(XIconSize *) NULL;
  number_sizes=0;
  root_window=XRootWindow(display,window->screen);
  if (XGetIconSizes(display,root_window,&size_list,&number_sizes) != 0)
    if ((number_sizes > 0) && (size_list != (XIconSize *) NULL))
      icon_size=size_list;
  if (icon_size == (XIconSize *) NULL)
    {
      /*
        Window manager does not restrict icon size.
      */
      icon_size=XAllocIconSize();
      if (icon_size == (XIconSize *) NULL)
        {
          ThrowXWindowException(ResourceLimitError,"MemoryAllocationFailed",
            image->filename);
          return;
        }
      icon_size->min_width=1;
      icon_size->max_width=MaxIconSize;
      icon_size->min_height=1;
      icon_size->max_height=MaxIconSize;
      icon_size->width_inc=1;
      icon_size->height_inc=1;
    }
  /*
    Determine aspect ratio of image.
  */
  width=(unsigned int) image->columns;
  height=(unsigned int) image->rows;
  i=0;
  if (window->crop_geometry)
    (void) XParseGeometry(window->crop_geometry,&i,&i,&width,&height);
  /*
    Look for an icon size that maintains the aspect ratio of image.
  */
  scale_factor=(double) icon_size->max_width/width;
  if (scale_factor > ((double) icon_size->max_height/height))
    scale_factor=(double) icon_size->max_height/height;
  icon_width=(unsigned int) icon_size->min_width;
  while ((int) icon_width < icon_size->max_width)
  {
    if (icon_width >= (unsigned int) (scale_factor*width+0.5))
      break;
    icon_width+=icon_size->width_inc;
  }
  icon_height=(unsigned int) icon_size->min_height;
  while ((int) icon_height < icon_size->max_height)
  {
    if (icon_height >= (unsigned int) (scale_factor*height+0.5))
      break;
    icon_height+=icon_size->height_inc;
  }
  (void) XFree((void *) icon_size);
  window->width=icon_width;
  window->height=icon_height;
}