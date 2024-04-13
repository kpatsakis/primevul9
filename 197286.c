MagickPrivate Window XWindowByProperty(Display *display,const Window window,
  const Atom property)
{
  Atom
    type;

  int
    format;

  Status
    status;

  unsigned char
    *data;

  unsigned int
    i,
    number_children;

  unsigned long
    after,
    number_items;

  Window
    child,
    *children,
    parent,
    root;

  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(window != (Window) NULL);
  assert(property != (Atom) NULL);
  status=XQueryTree(display,window,&root,&parent,&children,&number_children);
  if (status == False)
    return((Window) NULL);
  type=(Atom) NULL;
  child=(Window) NULL;
  for (i=0; (i < number_children) && (child == (Window) NULL); i++)
  {
    status=XGetWindowProperty(display,children[i],property,0L,0L,MagickFalse,
      (Atom) AnyPropertyType,&type,&format,&number_items,&after,&data);
    if (data != NULL)
      (void) XFree((void *) data);
    if ((status == Success) && (type != (Atom) NULL))
      child=children[i];
  }
  for (i=0; (i < number_children) && (child == (Window) NULL); i++)
    child=XWindowByProperty(display,children[i],property);
  if (children != (Window *) NULL)
    (void) XFree((void *) children);
  return(child);
}