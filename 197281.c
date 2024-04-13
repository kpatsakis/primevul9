MagickPrivate Window XWindowByID(Display *display,const Window root_window,
  const size_t id)
{
  RectangleInfo
    rectangle_info;

  register int
    i;

  Status
    status;

  unsigned int
    number_children;

  Window
    child,
    *children,
    window;

  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(root_window != (Window) NULL);
  if (id == 0)
    return(XSelectWindow(display,&rectangle_info));
  if (root_window == id)
    return(root_window);
  status=XQueryTree(display,root_window,&child,&child,&children,
    &number_children);
  if (status == False)
    return((Window) NULL);
  window=(Window) NULL;
  for (i=0; i < (int) number_children; i++)
  {
    /*
      Search each child and their children.
    */
    window=XWindowByID(display,children[i],id);
    if (window != (Window) NULL)
      break;
  }
  if (children != (Window *) NULL)
    (void) XFree((void *) children);
  return(window);
}