MagickPrivate Window XWindowByName(Display *display,const Window root_window,
  const char *name)
{
  register int
    i;

  Status
    status;

  unsigned int
    number_children;

  Window
    *children,
    child,
    window;

  XTextProperty
    window_name;

  assert(display != (Display *) NULL);
  assert(root_window != (Window) NULL);
  assert(name != (char *) NULL);
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",name);
  if (XGetWMName(display,root_window,&window_name) != 0)
    if (LocaleCompare((char *) window_name.value,name) == 0)
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
    window=XWindowByName(display,children[i],name);
    if (window != (Window) NULL)
      break;
  }
  if (children != (Window *) NULL)
    (void) XFree((void *) children);
  return(window);
}