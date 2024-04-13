static Window XGetSubwindow(Display *display,Window window,int x,int y)
{
  int
    x_offset,
    y_offset;

  Status
    status;

  Window
    source_window,
    target_window;

  assert(display != (Display *) NULL);
  source_window=XRootWindow(display,XDefaultScreen(display));
  if (window == (Window) NULL)
    return(source_window);
  target_window=window;
  for ( ; ; )
  {
    status=XTranslateCoordinates(display,source_window,window,x,y,
      &x_offset,&y_offset,&target_window);
    if (status != True)
      break;
    if (target_window == (Window) NULL)
      break;
    source_window=window;
    window=target_window;
    x=x_offset;
    y=y_offset;
  }
  if (target_window == (Window) NULL)
    target_window=window;
  return(target_window);
}