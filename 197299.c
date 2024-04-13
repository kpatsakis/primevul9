MagickPrivate XWindows *XInitializeWindows(Display *display,
  XResourceInfo *resource_info)
{
  Window
    root_window;

  XWindows
    *windows;

  /*
    Allocate windows structure.
  */
  windows=(XWindows *) AcquireMagickMemory(sizeof(*windows));
  if (windows == (XWindows *) NULL)
    {
      ThrowXWindowFatalException(XServerFatalError,"MemoryAllocationFailed",
        "...");
      return((XWindows *) NULL);
    }
  (void) memset(windows,0,sizeof(*windows));
  windows->pixel_info=(XPixelInfo *) AcquireMagickMemory(
    sizeof(*windows->pixel_info));
  windows->icon_pixel=(XPixelInfo *) AcquireMagickMemory(
    sizeof(*windows->icon_pixel));
  windows->icon_resources=(XResourceInfo *) AcquireMagickMemory(
    sizeof(*windows->icon_resources));
  if ((windows->pixel_info == (XPixelInfo *) NULL) ||
      (windows->icon_pixel == (XPixelInfo *) NULL) ||
      (windows->icon_resources == (XResourceInfo *) NULL))
    {
      ThrowXWindowFatalException(XServerFatalError,"MemoryAllocationFailed",
        "...");
      return((XWindows *) NULL);
    }
  /*
    Initialize windows structure.
  */
  windows->display=display;
  windows->wm_protocols=XInternAtom(display,"WM_PROTOCOLS",MagickFalse);
  windows->wm_delete_window=XInternAtom(display,"WM_DELETE_WINDOW",MagickFalse);
  windows->wm_take_focus=XInternAtom(display,"WM_TAKE_FOCUS",MagickFalse);
  windows->im_protocols=XInternAtom(display,"IM_PROTOCOLS",MagickFalse);
  windows->im_remote_command=
    XInternAtom(display,"IM_REMOTE_COMMAND",MagickFalse);
  windows->im_update_widget=XInternAtom(display,"IM_UPDATE_WIDGET",MagickFalse);
  windows->im_update_colormap=
    XInternAtom(display,"IM_UPDATE_COLORMAP",MagickFalse);
  windows->im_former_image=XInternAtom(display,"IM_FORMER_IMAGE",MagickFalse);
  windows->im_next_image=XInternAtom(display,"IM_NEXT_IMAGE",MagickFalse);
  windows->im_retain_colors=XInternAtom(display,"IM_RETAIN_COLORS",MagickFalse);
  windows->im_exit=XInternAtom(display,"IM_EXIT",MagickFalse);
  windows->dnd_protocols=XInternAtom(display,"DndProtocol",MagickFalse);
#if defined(MAGICKCORE_WINDOWS_SUPPORT)
  (void) XSynchronize(display,IsWindows95());
#endif
  if (IsEventLogging())
    {
      (void) XSynchronize(display,MagickTrue);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"Version: %s",
        GetMagickVersion((size_t *) NULL));
      (void) LogMagickEvent(X11Event,GetMagickModule(),"Protocols:");
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "  Window Manager: 0x%lx",windows->wm_protocols);
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "    delete window: 0x%lx",windows->wm_delete_window);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"    take focus: 0x%lx",
        windows->wm_take_focus);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"  ImageMagick: 0x%lx",
        windows->im_protocols);
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "    remote command: 0x%lx",windows->im_remote_command);
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "    update widget: 0x%lx",windows->im_update_widget);
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "    update colormap: 0x%lx",windows->im_update_colormap);
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "    former image: 0x%lx",windows->im_former_image);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"    next image: 0x%lx",
        windows->im_next_image);
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "    retain colors: 0x%lx",windows->im_retain_colors);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"    exit: 0x%lx",
        windows->im_exit);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"  Drag and Drop: 0x%lx",
        windows->dnd_protocols);
    }
  /*
    Allocate standard colormap.
  */
  windows->map_info=XAllocStandardColormap();
  windows->icon_map=XAllocStandardColormap();
  if ((windows->map_info == (XStandardColormap *) NULL) ||
      (windows->icon_map == (XStandardColormap *) NULL))
    ThrowXWindowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed",
      "...");
  windows->map_info->colormap=(Colormap) NULL;
  windows->icon_map->colormap=(Colormap) NULL;
  windows->pixel_info->pixels=(unsigned long *) NULL;
  windows->pixel_info->annotate_context=(GC) NULL;
  windows->pixel_info->highlight_context=(GC) NULL;
  windows->pixel_info->widget_context=(GC) NULL;
  windows->font_info=(XFontStruct *) NULL;
  windows->icon_pixel->annotate_context=(GC) NULL;
  windows->icon_pixel->pixels=(unsigned long *) NULL;
  /*
    Allocate visual.
  */
  *windows->icon_resources=(*resource_info);
  windows->icon_resources->visual_type=(char *) "default";
  windows->icon_resources->colormap=SharedColormap;
  windows->visual_info=
    XBestVisualInfo(display,windows->map_info,resource_info);
  windows->icon_visual=
    XBestVisualInfo(display,windows->icon_map,windows->icon_resources);
  if ((windows->visual_info == (XVisualInfo *) NULL) ||
      (windows->icon_visual == (XVisualInfo *) NULL))
    ThrowXWindowFatalException(XServerFatalError,"UnableToGetVisual",
      resource_info->visual_type);
  if (IsEventLogging())
    {
      (void) LogMagickEvent(X11Event,GetMagickModule(),"Visual:");
      (void) LogMagickEvent(X11Event,GetMagickModule(),"  visual id: 0x%lx",
        windows->visual_info->visualid);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"  class: %s",
        XVisualClassName(windows->visual_info->klass));
      (void) LogMagickEvent(X11Event,GetMagickModule(),"  depth: %d planes",
        windows->visual_info->depth);
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "  size of colormap: %d entries",windows->visual_info->colormap_size);
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "  red, green, blue masks: 0x%lx 0x%lx 0x%lx",
        windows->visual_info->red_mask,windows->visual_info->green_mask,
        windows->visual_info->blue_mask);
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "  significant bits in color: %d bits",
        windows->visual_info->bits_per_rgb);
    }
  /*
    Allocate class and manager hints.
  */
  windows->class_hints=XAllocClassHint();
  windows->manager_hints=XAllocWMHints();
  if ((windows->class_hints == (XClassHint *) NULL) ||
      (windows->manager_hints == (XWMHints *) NULL))
    ThrowXWindowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed",
      "...");
  /*
    Determine group leader if we have one.
  */
  root_window=XRootWindow(display,windows->visual_info->screen);
  windows->group_leader.id=(Window) NULL;
  if (resource_info->window_group != (char *) NULL)
    {
      if (isdigit((int) ((unsigned char) *resource_info->window_group)) != 0)
        windows->group_leader.id=XWindowByID(display,root_window,(Window)
          strtol((char *) resource_info->window_group,(char **) NULL,0));
      if (windows->group_leader.id == (Window) NULL)
        windows->group_leader.id=
          XWindowByName(display,root_window,resource_info->window_group);
    }
  return(windows);
}