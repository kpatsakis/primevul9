MagickExport void XQueryPosition(Display *display,const Window window,int *x,int *y)
{
  int
    x_root,
    y_root;

  unsigned int
    mask;

  Window
    root_window;

  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(window != (Window) NULL);
  assert(x != (int *) NULL);
  assert(y != (int *) NULL);
  (void) XQueryPointer(display,window,&root_window,&root_window,&x_root,&y_root,
    x,y,&mask);
}