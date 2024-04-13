MagickPrivate void XMakeWindow(Display *display,Window parent,char **argv,
  int argc,XClassHint *class_hint,XWMHints *manager_hints,
  XWindowInfo *window_info)
{
#define MinWindowSize  64

  Atom
    atom_list[2];

  int
    gravity;

  static XTextProperty
    icon_name,
    window_name;

  Status
    status;

  XSizeHints
    *size_hints;

  /*
    Set window info hints.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(window_info != (XWindowInfo *) NULL);
  size_hints=XAllocSizeHints();
  if (size_hints == (XSizeHints *) NULL)
    ThrowXWindowFatalException(XServerFatalError,"UnableToMakeXWindow",argv[0]);
  size_hints->flags=(int) window_info->flags;
  size_hints->x=window_info->x;
  size_hints->y=window_info->y;
  size_hints->width=(int) window_info->width;
  size_hints->height=(int) window_info->height;
  if (window_info->immutable != MagickFalse)
    {
      /*
        Window size cannot be changed.
      */
      size_hints->min_width=size_hints->width;
      size_hints->min_height=size_hints->height;
      size_hints->max_width=size_hints->width;
      size_hints->max_height=size_hints->height;
      size_hints->flags|=PMinSize;
      size_hints->flags|=PMaxSize;
    }
  else
    {
      /*
        Window size can be changed.
      */
      size_hints->min_width=(int) window_info->min_width;
      size_hints->min_height=(int) window_info->min_height;
      size_hints->flags|=PResizeInc;
      size_hints->width_inc=(int) window_info->width_inc;
      size_hints->height_inc=(int) window_info->height_inc;
#if !defined(PRE_R4_ICCCM)
      size_hints->flags|=PBaseSize;
      size_hints->base_width=size_hints->width_inc;
      size_hints->base_height=size_hints->height_inc;
#endif
    }
  gravity=NorthWestGravity;
  if (window_info->geometry != (char *) NULL)
    {
      char
        default_geometry[MagickPathExtent],
        geometry[MagickPathExtent];

      int
        flags;

      register char
        *p;

      /*
        User specified geometry.
      */
      (void) FormatLocaleString(default_geometry,MagickPathExtent,"%dx%d",
        size_hints->width,size_hints->height);
      (void) CopyMagickString(geometry,window_info->geometry,MagickPathExtent);
      p=geometry;
      while (strlen(p) != 0)
      {
        if ((isspace((int) ((unsigned char) *p)) == 0) && (*p != '%'))
          p++;
        else
          (void) memmove(p,p+1,MagickPathExtent-(p-geometry));
      }
      flags=XWMGeometry(display,window_info->screen,geometry,default_geometry,
        window_info->border_width,size_hints,&size_hints->x,&size_hints->y,
        &size_hints->width,&size_hints->height,&gravity);
      if ((flags & WidthValue) && (flags & HeightValue))
        size_hints->flags|=USSize;
      if ((flags & XValue) && (flags & YValue))
        {
          size_hints->flags|=USPosition;
          window_info->x=size_hints->x;
          window_info->y=size_hints->y;
        }
    }
#if !defined(PRE_R4_ICCCM)
  size_hints->win_gravity=gravity;
  size_hints->flags|=PWinGravity;
#endif
  if (window_info->id == (Window) NULL)
    window_info->id=XCreateWindow(display,parent,window_info->x,window_info->y,
      (unsigned int) size_hints->width,(unsigned int) size_hints->height,
      window_info->border_width,(int) window_info->depth,InputOutput,
      window_info->visual,(unsigned long) window_info->mask,
      &window_info->attributes);
  else
    {
      MagickStatusType
        mask;

      XEvent
        sans_event;

      XWindowChanges
        window_changes;

      /*
        Window already exists;  change relevant attributes.
      */
      (void) XChangeWindowAttributes(display,window_info->id,(unsigned long)
        window_info->mask,&window_info->attributes);
      mask=ConfigureNotify;
      while (XCheckTypedWindowEvent(display,window_info->id,(int) mask,&sans_event)) ;
      window_changes.x=window_info->x;
      window_changes.y=window_info->y;
      window_changes.width=(int) window_info->width;
      window_changes.height=(int) window_info->height;
      mask=(MagickStatusType) (CWWidth | CWHeight);
      if (window_info->flags & USPosition)
        mask|=CWX | CWY;
      (void) XReconfigureWMWindow(display,window_info->id,window_info->screen,
        mask,&window_changes);
    }
  if (window_info->id == (Window) NULL)
    ThrowXWindowFatalException(XServerFatalError,"UnableToCreateWindow",
      window_info->name);
  status=XStringListToTextProperty(&window_info->name,1,&window_name);
  if (status == False)
    ThrowXWindowFatalException(XServerFatalError,"UnableToCreateTextProperty",
      window_info->name);
  status=XStringListToTextProperty(&window_info->icon_name,1,&icon_name);
  if (status == False)
    ThrowXWindowFatalException(XServerFatalError,"UnableToCreateTextProperty",
      window_info->icon_name);
  if (window_info->icon_geometry != (char *) NULL)
    {
      int
        flags,
        height,
        width;

      /*
        User specified icon geometry.
      */
      size_hints->flags|=USPosition;
      flags=XWMGeometry(display,window_info->screen,window_info->icon_geometry,
        (char *) NULL,0,size_hints,&manager_hints->icon_x,
        &manager_hints->icon_y,&width,&height,&gravity);
      if ((flags & XValue) && (flags & YValue))
        manager_hints->flags|=IconPositionHint;
    }
  XSetWMProperties(display,window_info->id,&window_name,&icon_name,argv,argc,
    size_hints,manager_hints,class_hint);
  if (window_name.value != (void *) NULL)
    {
      (void) XFree((void *) window_name.value);
      window_name.value=(unsigned char *) NULL;
      window_name.nitems=0;
    }
  if (icon_name.value != (void *) NULL)
    {
      (void) XFree((void *) icon_name.value);
      icon_name.value=(unsigned char *) NULL;
      icon_name.nitems=0;
    }
  atom_list[0]=XInternAtom(display,"WM_DELETE_WINDOW",MagickFalse);
  atom_list[1]=XInternAtom(display,"WM_TAKE_FOCUS",MagickFalse);
  (void) XSetWMProtocols(display,window_info->id,atom_list,2);
  (void) XFree((void *) size_hints);
  if (window_info->shape != MagickFalse)
    {
#if defined(MAGICKCORE_HAVE_SHAPE)
      int
        error_base,
        event_base;

      /*
        Can we apply a non-rectangular shaping mask?
      */
      error_base=0;
      event_base=0;
      if (XShapeQueryExtension(display,&error_base,&event_base) == 0)
        window_info->shape=MagickFalse;
#else
      window_info->shape=MagickFalse;
#endif
    }
  if (window_info->shared_memory)
    {
#if defined(MAGICKCORE_HAVE_SHARED_MEMORY)
      /*
        Can we use shared memory with this window?
      */
      if (XShmQueryExtension(display) == 0)
        window_info->shared_memory=MagickFalse;
#else
      window_info->shared_memory=MagickFalse;
#endif
    }
  window_info->image=NewImageList();
  window_info->destroy=MagickFalse;
}