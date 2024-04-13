static Window XClientWindow(Display *display,Window target_window)
{
  Atom
    state,
    type;

  int
    format;

  Status
    status;

  unsigned char
    *data;

  unsigned long
    after,
    number_items;

  Window
    client_window;

  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  state=XInternAtom(display,"WM_STATE",MagickTrue);
  if (state == (Atom) NULL)
    return(target_window);
  type=(Atom) NULL;
  status=XGetWindowProperty(display,target_window,state,0L,0L,MagickFalse,
    (Atom) AnyPropertyType,&type,&format,&number_items,&after,&data);
  if ((status == Success) && (type != (Atom) NULL))
    return(target_window);
  client_window=XWindowByProperty(display,target_window,state);
  if (client_window == (Window) NULL)
    return(target_window);
  return(client_window);
}