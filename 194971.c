MagickExport Image *XAnimateImages(Display *display,
  XResourceInfo *resource_info,char **argv,const int argc,Image *images)
{
#define MagickMenus  4
#define MaXWindows  8
#define MagickTitle  "Commands"

  static const char
    *CommandMenu[]=
    {
      "Animate",
      "Speed",
      "Direction",
      "Help",
      "Image Info",
      "Quit",
      (char *) NULL
    },
    *AnimateMenu[]=
    {
      "Open...",
      "Play",
      "Step",
      "Repeat",
      "Auto Reverse",
      "Save...",
      (char *) NULL
    },
    *SpeedMenu[]=
    {
      "Faster",
      "Slower",
      (char *) NULL
    },
    *DirectionMenu[]=
    {
      "Forward",
      "Reverse",
      (char *) NULL
    },
    *HelpMenu[]=
    {
      "Overview",
      "Browse Documentation",
      "About Animate",
      (char *) NULL
    };

  static const char
    **Menus[MagickMenus]=
    {
      AnimateMenu,
      SpeedMenu,
      DirectionMenu,
      HelpMenu
    };

  static const CommandType
    CommandMenus[]=
    {
      NullCommand,
      NullCommand,
      NullCommand,
      NullCommand,
      InfoCommand,
      QuitCommand
    },
    CommandTypes[]=
    {
      OpenCommand,
      PlayCommand,
      StepCommand,
      RepeatCommand,
      AutoReverseCommand,
      SaveCommand
    },
    SpeedCommands[]=
    {
      FasterCommand,
      SlowerCommand
    },
    DirectionCommands[]=
    {
      ForwardCommand,
      ReverseCommand
    },
    HelpCommands[]=
    {
      HelpCommand,
      BrowseDocumentationCommand,
      VersionCommand
    };

  static const CommandType
    *Commands[MagickMenus]=
    {
      CommandTypes,
      SpeedCommands,
      DirectionCommands,
      HelpCommands
    };

  char
    command[MaxTextExtent],
    *directory,
    geometry[MaxTextExtent],
    resource_name[MaxTextExtent];

  CommandType
    command_type;

  Image
    *coalesce_image,
    *display_image,
    *image,
    **image_list,
    *nexus;

  int
    status;

  KeySym
    key_symbol;

  MagickStatusType
    context_mask,
    state;

  RectangleInfo
    geometry_info;

  register char
    *p;

  register ssize_t
    i;

  ssize_t
    first_scene,
    iterations,
    scene;

  static char
    working_directory[MaxTextExtent];

  static size_t
    number_windows;

  static XWindowInfo
    *magick_windows[MaXWindows];

  time_t
    timestamp;

  size_t
    delay,
    number_scenes;

  WarningHandler
    warning_handler;

  Window
    root_window;

  XClassHint
    *class_hints;

  XEvent
    event;

  XFontStruct
    *font_info;

  XGCValues
    context_values;

  XPixelInfo
    *icon_pixel,
    *pixel;

  XResourceInfo
    *icon_resources;

  XStandardColormap
    *icon_map,
    *map_info;

  XTextProperty
    window_name;

  XVisualInfo
    *icon_visual,
    *visual_info;

  XWindowChanges
    window_changes;

  XWindows
    *windows;

  XWMHints
    *manager_hints;

  assert(images != (Image *) NULL);
  assert(images->signature == MagickCoreSignature);
  if (images->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",images->filename);
  warning_handler=(WarningHandler) NULL;
  windows=XSetWindows((XWindows *) ~0);
  if (windows != (XWindows *) NULL)
    {
      int
        status;

      if (*working_directory == '\0')
        (void) CopyMagickString(working_directory,".",MaxTextExtent);
      status=chdir(working_directory);
      if (status == -1)
        (void) ThrowMagickException(&images->exception,GetMagickModule(),
          FileOpenError,"UnableToOpenFile","%s",working_directory);
      warning_handler=resource_info->display_warnings ?
        SetErrorHandler(XWarning) : SetErrorHandler((ErrorHandler) NULL);
      warning_handler=resource_info->display_warnings ?
        SetWarningHandler(XWarning) : SetWarningHandler((WarningHandler) NULL);
    }
  else
    {
      register Image
        *p;

      /*
        Initialize window structure.
      */
      for (p=images; p != (Image *) NULL; p=GetNextImageInList(p))
      {
        if (p->storage_class == DirectClass)
          {
            resource_info->colors=0;
            break;
          }
        if (p->colors > resource_info->colors)
          resource_info->colors=p->colors;
      }
      windows=XSetWindows(XInitializeWindows(display,resource_info));
      if (windows == (XWindows *) NULL)
        ThrowXWindowFatalException(XServerFatalError,"MemoryAllocationFailed",
          images->filename);
      /*
        Initialize window id's.
      */
      number_windows=0;
      magick_windows[number_windows++]=(&windows->icon);
      magick_windows[number_windows++]=(&windows->backdrop);
      magick_windows[number_windows++]=(&windows->image);
      magick_windows[number_windows++]=(&windows->info);
      magick_windows[number_windows++]=(&windows->command);
      magick_windows[number_windows++]=(&windows->widget);
      magick_windows[number_windows++]=(&windows->popup);
      for (i=0; i < (ssize_t) number_windows; i++)
        magick_windows[i]->id=(Window) NULL;
    }
  /*
    Initialize font info.
  */
  if (windows->font_info != (XFontStruct *) NULL)
    (void) XFreeFont(display,windows->font_info);
  windows->font_info=XBestFont(display,resource_info,MagickFalse);
  if (windows->font_info == (XFontStruct *) NULL)
    ThrowXWindowFatalException(XServerFatalError,"UnableToLoadFont",
      resource_info->font);
  /*
    Initialize Standard Colormap.
  */
  map_info=windows->map_info;
  icon_map=windows->icon_map;
  visual_info=windows->visual_info;
  icon_visual=windows->icon_visual;
  pixel=windows->pixel_info;
  icon_pixel=windows->icon_pixel;
  font_info=windows->font_info;
  icon_resources=windows->icon_resources;
  class_hints=windows->class_hints;
  manager_hints=windows->manager_hints;
  root_window=XRootWindow(display,visual_info->screen);
  coalesce_image=CoalesceImages(images,&images->exception);
  if (coalesce_image == (Image *) NULL)
    ThrowXWindowFatalException(XServerFatalError,"MemoryAllocationFailed",
      images->filename);
  images=coalesce_image;
  if (resource_info->map_type == (char *) NULL)
    if ((visual_info->klass != TrueColor) &&
        (visual_info->klass != DirectColor))
      {
        Image
          *next;

        /*
          Determine if the sequence of images has the identical colormap.
        */
        for (next=images; next != (Image *) NULL; )
        {
          next->matte=MagickFalse;
          if ((next->storage_class == DirectClass) ||
              (next->colors != images->colors) ||
              (next->colors > (size_t) visual_info->colormap_size))
            break;
          for (i=0; i < (ssize_t) images->colors; i++)
            if (IsColorEqual(next->colormap+i,images->colormap+i) == MagickFalse)
              break;
          if (i < (ssize_t) images->colors)
            break;
          next=GetNextImageInList(next);
        }
        if (next != (Image *) NULL)
          (void) RemapImages(resource_info->quantize_info,images,
            (Image *) NULL);
      }
  /*
    Sort images by increasing scene number.
  */
  number_scenes=GetImageListLength(images);
  image_list=ImageListToArray(images,&images->exception);
  if (image_list == (Image **) NULL)
    ThrowXWindowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed",
      images->filename);
  for (scene=0; scene < (ssize_t) number_scenes; scene++)
    if (image_list[scene]->scene == 0)
      break;
  if (scene == (ssize_t) number_scenes)
    qsort((void *) image_list,number_scenes,sizeof(Image *),SceneCompare);
  /*
    Initialize Standard Colormap.
  */
  nexus=NewImageList();
  display_image=image_list[0];
  for (scene=0; scene < (ssize_t) number_scenes; scene++)
  {
    if ((resource_info->map_type != (char *) NULL) ||
        (visual_info->klass == TrueColor) ||
        (visual_info->klass == DirectColor))
      (void) SetImageType(image_list[scene],image_list[scene]->matte ==
        MagickFalse ? TrueColorType : TrueColorMatteType);
    if ((display_image->columns < image_list[scene]->columns) &&
        (display_image->rows < image_list[scene]->rows))
      display_image=image_list[scene];
  }
  if (display_image->debug != MagickFalse)
    {
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "Image: %s[%.20g] %.20gx%.20g ",display_image->filename,(double)
        display_image->scene,(double) display_image->columns,(double)
        display_image->rows);
      if (display_image->colors != 0)
        (void) LogMagickEvent(X11Event,GetMagickModule(),"%.20gc ",(double)
          display_image->colors);
      (void) LogMagickEvent(X11Event,GetMagickModule(),"%s",
        display_image->magick);
    }
  XMakeStandardColormap(display,visual_info,resource_info,display_image,
    map_info,pixel);
  /*
    Initialize graphic context.
  */
  windows->context.id=(Window) NULL;
  XGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->context);
  (void) CloneString(&class_hints->res_name,resource_info->client_name);
  (void) CloneString(&class_hints->res_class,resource_info->client_name);
  class_hints->res_class[0]=(char) toupper((int) class_hints->res_class[0]);
  manager_hints->flags=InputHint | StateHint;
  manager_hints->input=MagickFalse;
  manager_hints->initial_state=WithdrawnState;
  XMakeWindow(display,root_window,argv,argc,class_hints,manager_hints,
    &windows->context);
  if (display_image->debug != MagickFalse)
    (void) LogMagickEvent(X11Event,GetMagickModule(),
      "Window id: 0x%lx (context)",windows->context.id);
  context_values.background=pixel->background_color.pixel;
  context_values.font=font_info->fid;
  context_values.foreground=pixel->foreground_color.pixel;
  context_values.graphics_exposures=MagickFalse;
  context_mask=(MagickStatusType)
    (GCBackground | GCFont | GCForeground | GCGraphicsExposures);
  if (pixel->annotate_context != (GC) NULL)
    (void) XFreeGC(display,pixel->annotate_context);
  pixel->annotate_context=
    XCreateGC(display,windows->context.id,context_mask,&context_values);
  if (pixel->annotate_context == (GC) NULL)
    ThrowXWindowFatalException(XServerFatalError,"UnableToCreateGraphicContext",
      images->filename);
  context_values.background=pixel->depth_color.pixel;
  if (pixel->widget_context != (GC) NULL)
    (void) XFreeGC(display,pixel->widget_context);
  pixel->widget_context=
    XCreateGC(display,windows->context.id,context_mask,&context_values);
  if (pixel->widget_context == (GC) NULL)
    ThrowXWindowFatalException(XServerFatalError,"UnableToCreateGraphicContext",
      images->filename);
  context_values.background=pixel->foreground_color.pixel;
  context_values.foreground=pixel->background_color.pixel;
  context_values.plane_mask=
    context_values.background ^ context_values.foreground;
  if (pixel->highlight_context != (GC) NULL)
    (void) XFreeGC(display,pixel->highlight_context);
  pixel->highlight_context=XCreateGC(display,windows->context.id,
    (size_t) (context_mask | GCPlaneMask),&context_values);
  if (pixel->highlight_context == (GC) NULL)
    ThrowXWindowFatalException(XServerFatalError,"UnableToCreateGraphicContext",
      images->filename);
  (void) XDestroyWindow(display,windows->context.id);
  /*
    Initialize icon window.
  */
  XGetWindowInfo(display,icon_visual,icon_map,icon_pixel,(XFontStruct *) NULL,
    icon_resources,&windows->icon);
  windows->icon.geometry=resource_info->icon_geometry;
  XBestIconSize(display,&windows->icon,display_image);
  windows->icon.attributes.colormap=
    XDefaultColormap(display,icon_visual->screen);
  windows->icon.attributes.event_mask=ExposureMask | StructureNotifyMask;
  manager_hints->flags=InputHint | StateHint;
  manager_hints->input=MagickFalse;
  manager_hints->initial_state=IconicState;
  XMakeWindow(display,root_window,argv,argc,class_hints,manager_hints,
    &windows->icon);
  if (display_image->debug != MagickFalse)
    (void) LogMagickEvent(X11Event,GetMagickModule(),"Window id: 0x%lx (icon)",
      windows->icon.id);
  /*
    Initialize graphic context for icon window.
  */
  if (icon_pixel->annotate_context != (GC) NULL)
    (void) XFreeGC(display,icon_pixel->annotate_context);
  context_values.background=icon_pixel->background_color.pixel;
  context_values.foreground=icon_pixel->foreground_color.pixel;
  icon_pixel->annotate_context=XCreateGC(display,windows->icon.id,
    (size_t) (GCBackground | GCForeground),&context_values);
  if (icon_pixel->annotate_context == (GC) NULL)
    ThrowXWindowFatalException(XServerFatalError,"UnableToCreateGraphicContext",
      images->filename);
  windows->icon.annotate_context=icon_pixel->annotate_context;
  /*
    Initialize Image window.
  */
  XGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->image);
  windows->image.shape=MagickTrue;  /* non-rectangular shape hint */
  if (resource_info->use_shared_memory == MagickFalse)
    windows->image.shared_memory=MagickFalse;
  if (resource_info->title != (char *) NULL)
    {
      char
        *title;

      title=InterpretImageProperties(resource_info->image_info,display_image,
        resource_info->title);
      (void) CopyMagickString(windows->image.name,title,MaxTextExtent);
      (void) CopyMagickString(windows->image.icon_name,title,MaxTextExtent);
      title=DestroyString(title);
    }
  else
    {
      char
        filename[MaxTextExtent];

      /*
        Window name is the base of the filename.
      */
      GetPathComponent(display_image->magick_filename,TailPath,filename);
      (void) FormatLocaleString(windows->image.name,MaxTextExtent,
        "%s: %s[scene: %.20g frames: %.20g]",MagickPackageName,filename,(double)
        display_image->scene,(double) number_scenes);
      (void) CopyMagickString(windows->image.icon_name,filename,MaxTextExtent);
    }
  if (resource_info->immutable != MagickFalse)
    windows->image.immutable=MagickTrue;
  windows->image.shape=MagickTrue;
  windows->image.geometry=resource_info->image_geometry;
  (void) FormatLocaleString(geometry,MaxTextExtent,"%ux%u+0+0>!",
    XDisplayWidth(display,visual_info->screen),
    XDisplayHeight(display,visual_info->screen));
  geometry_info.width=display_image->columns;
  geometry_info.height=display_image->rows;
  geometry_info.x=0;
  geometry_info.y=0;
  (void) ParseMetaGeometry(geometry,&geometry_info.x,&geometry_info.y,
    &geometry_info.width,&geometry_info.height);
  windows->image.width=(unsigned int) geometry_info.width;
  windows->image.height=(unsigned int) geometry_info.height;
  windows->image.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | EnterWindowMask | ExposureMask | KeyPressMask |
    KeyReleaseMask | LeaveWindowMask | OwnerGrabButtonMask |
    PropertyChangeMask | StructureNotifyMask | SubstructureNotifyMask;
  XGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->backdrop);
  if ((resource_info->backdrop) || (windows->backdrop.id != (Window) NULL))
    {
      /*
        Initialize backdrop window.
      */
      windows->backdrop.x=0;
      windows->backdrop.y=0;
      (void) CloneString(&windows->backdrop.name,"ImageMagick Backdrop");
      windows->backdrop.flags=(size_t) (USSize | USPosition);
      windows->backdrop.width=(unsigned int)
        XDisplayWidth(display,visual_info->screen);
      windows->backdrop.height=(unsigned int)
        XDisplayHeight(display,visual_info->screen);
      windows->backdrop.border_width=0;
      windows->backdrop.immutable=MagickTrue;
      windows->backdrop.attributes.do_not_propagate_mask=ButtonPressMask |
        ButtonReleaseMask;
      windows->backdrop.attributes.event_mask=ButtonPressMask | KeyPressMask |
        StructureNotifyMask;
      manager_hints->flags=IconWindowHint | InputHint | StateHint;
      manager_hints->icon_window=windows->icon.id;
      manager_hints->input=MagickTrue;
      manager_hints->initial_state=
        resource_info->iconic ? IconicState : NormalState;
      XMakeWindow(display,root_window,argv,argc,class_hints,manager_hints,
        &windows->backdrop);
      if (display_image->debug != MagickFalse)
        (void) LogMagickEvent(X11Event,GetMagickModule(),
          "Window id: 0x%lx (backdrop)",windows->backdrop.id);
      (void) XMapWindow(display,windows->backdrop.id);
      (void) XClearWindow(display,windows->backdrop.id);
      if (windows->image.id != (Window) NULL)
        {
          (void) XDestroyWindow(display,windows->image.id);
          windows->image.id=(Window) NULL;
        }
      /*
        Position image in the center the backdrop.
      */
      windows->image.flags|=USPosition;
      windows->image.x=(XDisplayWidth(display,visual_info->screen)/2)-
        (windows->image.width/2);
      windows->image.y=(XDisplayHeight(display,visual_info->screen)/2)-
        (windows->image.height/2);
    }
  manager_hints->flags=IconWindowHint | InputHint | StateHint;
  manager_hints->icon_window=windows->icon.id;
  manager_hints->input=MagickTrue;
  manager_hints->initial_state=
    resource_info->iconic ? IconicState : NormalState;
  if (windows->group_leader.id != (Window) NULL)
    {
      /*
        Follow the leader.
      */
      manager_hints->flags|=(MagickStatusType) WindowGroupHint;
      manager_hints->window_group=windows->group_leader.id;
      (void) XSelectInput(display,windows->group_leader.id,StructureNotifyMask);
      if (display_image->debug != MagickFalse)
        (void) LogMagickEvent(X11Event,GetMagickModule(),
          "Window id: 0x%lx (group leader)",windows->group_leader.id);
    }
  XMakeWindow(display,
    (Window) (resource_info->backdrop ? windows->backdrop.id : root_window),
    argv,argc,class_hints,manager_hints,&windows->image);
  (void) XChangeProperty(display,windows->image.id,windows->im_protocols,
    XA_STRING,8,PropModeReplace,(unsigned char *) NULL,0);
  if (windows->group_leader.id != (Window) NULL)
    (void) XSetTransientForHint(display,windows->image.id,
      windows->group_leader.id);
  if (display_image->debug != MagickFalse)
    (void) LogMagickEvent(X11Event,GetMagickModule(),"Window id: 0x%lx (image)",
      windows->image.id);
  /*
    Initialize Info widget.
  */
  XGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->info);
  (void) CloneString(&windows->info.name,"Info");
  (void) CloneString(&windows->info.icon_name,"Info");
  windows->info.border_width=1;
  windows->info.x=2;
  windows->info.y=2;
  windows->info.flags|=PPosition;
  windows->info.attributes.win_gravity=UnmapGravity;
  windows->info.attributes.event_mask=ButtonPressMask | ExposureMask |
    StructureNotifyMask;
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=MagickFalse;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=windows->image.id;
  XMakeWindow(display,windows->image.id,argv,argc,class_hints,manager_hints,
    &windows->info);
  windows->info.highlight_stipple=XCreateBitmapFromData(display,
    windows->info.id,(char *) HighlightBitmap,HighlightWidth,HighlightHeight);
  windows->info.shadow_stipple=XCreateBitmapFromData(display,
    windows->info.id,(char *) ShadowBitmap,ShadowWidth,ShadowHeight);
  (void) XSetTransientForHint(display,windows->info.id,windows->image.id);
  if (windows->image.mapped)
    (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
  if (display_image->debug != MagickFalse)
    (void) LogMagickEvent(X11Event,GetMagickModule(),"Window id: 0x%lx (info)",
      windows->info.id);
  /*
    Initialize Command widget.
  */
  XGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->command);
  windows->command.data=MagickMenus;
  (void) XCommandWidget(display,windows,CommandMenu,(XEvent *) NULL);
  (void) FormatLocaleString(resource_name,MaxTextExtent,"%s.command",
    resource_info->client_name);
  windows->command.geometry=XGetResourceClass(resource_info->resource_database,
    resource_name,"geometry",(char *) NULL);
  (void) CloneString(&windows->command.name,MagickTitle);
  windows->command.border_width=0;
  windows->command.flags|=PPosition;
  windows->command.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | EnterWindowMask | ExposureMask | LeaveWindowMask |
    OwnerGrabButtonMask | StructureNotifyMask;
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=MagickTrue;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=windows->image.id;
  XMakeWindow(display,root_window,argv,argc,class_hints,manager_hints,
    &windows->command);
  windows->command.highlight_stipple=XCreateBitmapFromData(display,
    windows->command.id,(char *) HighlightBitmap,HighlightWidth,
    HighlightHeight);
  windows->command.shadow_stipple=XCreateBitmapFromData(display,
    windows->command.id,(char *) ShadowBitmap,ShadowWidth,ShadowHeight);
  (void) XSetTransientForHint(display,windows->command.id,windows->image.id);
  if (display_image->debug != MagickFalse)
    (void) LogMagickEvent(X11Event,GetMagickModule(),
      "Window id: 0x%lx (command)",windows->command.id);
  /*
    Initialize Widget window.
  */
  XGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->widget);
  (void) FormatLocaleString(resource_name,MaxTextExtent,"%s.widget",
    resource_info->client_name);
  windows->widget.geometry=XGetResourceClass(resource_info->resource_database,
    resource_name,"geometry",(char *) NULL);
  windows->widget.border_width=0;
  windows->widget.flags|=PPosition;
  windows->widget.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | EnterWindowMask | ExposureMask | KeyPressMask |
    KeyReleaseMask | LeaveWindowMask | OwnerGrabButtonMask |
    StructureNotifyMask;
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=MagickTrue;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=windows->image.id;
  XMakeWindow(display,root_window,argv,argc,class_hints,manager_hints,
    &windows->widget);
  windows->widget.highlight_stipple=XCreateBitmapFromData(display,
    windows->widget.id,(char *) HighlightBitmap,HighlightWidth,HighlightHeight);
  windows->widget.shadow_stipple=XCreateBitmapFromData(display,
    windows->widget.id,(char *) ShadowBitmap,ShadowWidth,ShadowHeight);
  (void) XSetTransientForHint(display,windows->widget.id,windows->image.id);
  if (display_image->debug != MagickFalse)
    (void) LogMagickEvent(X11Event,GetMagickModule(),
      "Window id: 0x%lx (widget)",windows->widget.id);
  /*
    Initialize popup window.
  */
  XGetWindowInfo(display,visual_info,map_info,pixel,font_info,
    resource_info,&windows->popup);
  windows->popup.border_width=0;
  windows->popup.flags|=PPosition;
  windows->popup.attributes.event_mask=ButtonMotionMask | ButtonPressMask |
    ButtonReleaseMask | EnterWindowMask | ExposureMask | KeyPressMask |
    KeyReleaseMask | LeaveWindowMask | StructureNotifyMask;
  manager_hints->flags=InputHint | StateHint | WindowGroupHint;
  manager_hints->input=MagickTrue;
  manager_hints->initial_state=NormalState;
  manager_hints->window_group=windows->image.id;
  XMakeWindow(display,root_window,argv,argc,class_hints,manager_hints,
    &windows->popup);
  windows->popup.highlight_stipple=XCreateBitmapFromData(display,
    windows->popup.id,(char *) HighlightBitmap,HighlightWidth,HighlightHeight);
  windows->popup.shadow_stipple=XCreateBitmapFromData(display,
    windows->popup.id,(char *) ShadowBitmap,ShadowWidth,ShadowHeight);
  (void) XSetTransientForHint(display,windows->popup.id,windows->image.id);
  if (display_image->debug != MagickFalse)
    (void) LogMagickEvent(X11Event,GetMagickModule(),
      "Window id: 0x%lx (pop up)",windows->popup.id);
  /*
    Set out progress and warning handlers.
  */
  if (warning_handler == (WarningHandler) NULL)
    {
      warning_handler=resource_info->display_warnings ?
        SetErrorHandler(XWarning) : SetErrorHandler((ErrorHandler) NULL);
      warning_handler=resource_info->display_warnings ?
        SetWarningHandler(XWarning) : SetWarningHandler((WarningHandler) NULL);
    }
  /*
    Initialize X image structure.
  */
  windows->image.x=0;
  windows->image.y=0;
  /*
    Initialize image pixmaps structure.
  */
  window_changes.width=(int) windows->image.width;
  window_changes.height=(int) windows->image.height;
  (void) XReconfigureWMWindow(display,windows->image.id,windows->command.screen,
    (unsigned int) (CWWidth | CWHeight),&window_changes);
  windows->image.pixmaps=(Pixmap *) AcquireQuantumMemory(number_scenes,
    sizeof(*windows->image.pixmaps));
  windows->image.matte_pixmaps=(Pixmap *) AcquireQuantumMemory(number_scenes,
    sizeof(*windows->image.pixmaps));
  if ((windows->image.pixmaps == (Pixmap *) NULL) ||
      (windows->image.matte_pixmaps == (Pixmap *) NULL))
    ThrowXWindowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed",
      images->filename);
  if ((windows->image.mapped == MagickFalse) ||
      (windows->backdrop.id != (Window) NULL))
    (void) XMapWindow(display,windows->image.id);
  XSetCursorState(display,windows,MagickTrue);
  for (scene=0; scene < (ssize_t) number_scenes; scene++)
  {
    unsigned int
      columns,
      rows;

    /*
      Create X image.
    */
    windows->image.pixmap=(Pixmap) NULL;
    windows->image.matte_pixmap=(Pixmap) NULL;
    if ((resource_info->map_type != (char *) NULL) ||
        (visual_info->klass == TrueColor) ||
        (visual_info->klass == DirectColor))
      if (image_list[scene]->storage_class == PseudoClass)
        XGetPixelPacket(display,visual_info,map_info,resource_info,
          image_list[scene],windows->image.pixel_info);
    columns=(unsigned int) image_list[scene]->columns;
    rows=(unsigned int) image_list[scene]->rows;
    if ((image_list[scene]->columns != columns) ||
        (image_list[scene]->rows != rows))
      ThrowXWindowFatalException(XServerFatalError,"UnableToCreateXImage",
        image_list[scene]->filename);
    status=XMakeImage(display,resource_info,&windows->image,image_list[scene],
      columns,rows);
    if (status == MagickFalse)
      ThrowXWindowFatalException(XServerFatalError,"UnableToCreateXImage",
        images->filename);
    if (image_list[scene]->debug != MagickFalse)
      {
        (void) LogMagickEvent(X11Event,GetMagickModule(),
          "Image: [%.20g] %s %.20gx%.20g ",(double) image_list[scene]->scene,
          image_list[scene]->filename,(double) columns,(double) rows);
        if (image_list[scene]->colors != 0)
          (void) LogMagickEvent(X11Event,GetMagickModule(),"%.20gc ",(double)
            image_list[scene]->colors);
        (void) LogMagickEvent(X11Event,GetMagickModule(),"%s",
          image_list[scene]->magick);
      }
    /*
      Window name is the base of the filename.
    */
    if (resource_info->title != (char *) NULL)
      {
        char
          *title;

        title=InterpretImageProperties(resource_info->image_info,
          image_list[scene],resource_info->title);
        (void) CopyMagickString(windows->image.name,title,MaxTextExtent);
        title=DestroyString(title);
      }
    else
      {
        p=image_list[scene]->magick_filename+
          strlen(image_list[scene]->magick_filename)-1;
        while ((p > image_list[scene]->magick_filename) && (*(p-1) != '/'))
          p--;
        (void) FormatLocaleString(windows->image.name,MaxTextExtent,
          "%s: %s[%.20g of %.20g]",MagickPackageName,p,(double) scene+1,
          (double) number_scenes);
      }
    status=XStringListToTextProperty(&windows->image.name,1,&window_name);
    if (status != Success)
      {
        XSetWMName(display,windows->image.id,&window_name);
        (void) XFree((void *) window_name.value);
      }
    windows->image.pixmaps[scene]=windows->image.pixmap;
    windows->image.matte_pixmaps[scene]=windows->image.matte_pixmap;
    if (scene == 0)
      {
        event.xexpose.x=0;
        event.xexpose.y=0;
        event.xexpose.width=(int) image_list[scene]->columns;
        event.xexpose.height=(int) image_list[scene]->rows;
        XRefreshWindow(display,&windows->image,&event);
        (void) XSync(display,MagickFalse);
    }
  }
  XSetCursorState(display,windows,MagickFalse);
  if (windows->command.mapped)
    (void) XMapRaised(display,windows->command.id);
  /*
    Respond to events.
  */
  nexus=NewImageList();
  scene=0;
  first_scene=0;
  iterations=0;
  image=image_list[0];
  state=(MagickStatusType) (ForwardAnimationState | RepeatAnimationState);
  (void) XMagickCommand(display,resource_info,windows,PlayCommand,&images,
    &state);
  do
  {
    if (XEventsQueued(display,QueuedAfterFlush) == 0)
      if ((state & PlayAnimationState) || (state & StepAnimationState))
        {
          MagickBooleanType
            pause;

          pause=MagickFalse;
          delay=1000*image->delay/MagickMax(image->ticks_per_second,1L);
          XDelay(display,resource_info->delay*(delay == 0 ? 10 : delay));
          if (state & ForwardAnimationState)
            {
              /*
                Forward animation:  increment scene number.
              */
              if (scene < ((ssize_t) number_scenes-1))
                scene++;
              else
                {
                  iterations++;
                  if (iterations == (ssize_t) image_list[0]->iterations)
                    {
                      iterations=0;
                      state|=ExitState;
                    }
                  if ((state & AutoReverseAnimationState) != 0)
                    {
                      state&=(~ForwardAnimationState);
                      scene--;
                    }
                  else
                    {
                      if ((state & RepeatAnimationState) == 0)
                        state&=(~PlayAnimationState);
                      scene=first_scene;
                      pause=MagickTrue;
                    }
                }
            }
          else
            {
              /*
                Reverse animation:  decrement scene number.
              */
              if (scene > first_scene)
                scene--;
              else
                {
                  iterations++;
                  if (iterations == (ssize_t) image_list[0]->iterations)
                    {
                      iterations=0;
                      state&=(~RepeatAnimationState);
                    }
                  if (state & AutoReverseAnimationState)
                    {
                      state|=ForwardAnimationState;
                      scene=first_scene;
                      pause=MagickTrue;
                    }
                  else
                    {
                      if ((state & RepeatAnimationState) == MagickFalse)
                        state&=(~PlayAnimationState);
                      scene=(ssize_t) number_scenes-1;
                    }
                }
            }
          scene=MagickMax(scene,0);
          image=image_list[scene];
          if ((image != (Image *) NULL) && (image->start_loop != 0))
            first_scene=scene;
          if ((state & StepAnimationState) ||
              (resource_info->title != (char *) NULL))
            {
              /*
                Update window title.
              */
              p=image_list[scene]->filename+
                strlen(image_list[scene]->filename)-1;
              while ((p > image_list[scene]->filename) && (*(p-1) != '/'))
                p--;
              (void) FormatLocaleString(windows->image.name,MaxTextExtent,
                "%s: %s[%.20g of %.20g]",MagickPackageName,p,(double)
                scene+1,(double) number_scenes);
              if (resource_info->title != (char *) NULL)
                {
                  char
                    *title;

                  title=InterpretImageProperties(resource_info->image_info,
                    image,resource_info->title);
                  (void) CopyMagickString(windows->image.name,title,
                    MaxTextExtent);
                  title=DestroyString(title);
                }
              status=XStringListToTextProperty(&windows->image.name,1,
                &window_name);
              if (status != Success)
                {
                  XSetWMName(display,windows->image.id,&window_name);
                  (void) XFree((void *) window_name.value);
                }
            }
          /*
            Copy X pixmap to Image window.
          */
          XGetPixelPacket(display,visual_info,map_info,resource_info,
            image_list[scene],windows->image.pixel_info);
          windows->image.ximage->width=(int) image->columns;
          windows->image.ximage->height=(int) image->rows;
          windows->image.pixmap=windows->image.pixmaps[scene];
          windows->image.matte_pixmap=windows->image.matte_pixmaps[scene];
          event.xexpose.x=0;
          event.xexpose.y=0;
          event.xexpose.width=(int) image->columns;
          event.xexpose.height=(int) image->rows;
          if ((state & ExitState) == 0)
            {
              XRefreshWindow(display,&windows->image,&event);
              (void) XSync(display,MagickFalse);
            }
          state&=(~StepAnimationState);
          if (pause != MagickFalse)
            for (i=0; i < (ssize_t) resource_info->pause; i++)
            {
              int
                status;

              status=XCheckTypedWindowEvent(display,windows->image.id,KeyPress,
                &event);
              if (status != 0)
                {
                  int
                    length;

                  length=XLookupString((XKeyEvent *) &event.xkey,command,(int)
                    sizeof(command),&key_symbol,(XComposeStatus *) NULL);
                  *(command+length)='\0';
                  if ((key_symbol == XK_q) || (key_symbol == XK_Escape))
                    {
                      XClientMessage(display,windows->image.id,
                        windows->im_protocols,windows->im_exit,CurrentTime);
                      break;
                    }
                }
              MagickDelay(1000);
            }
          continue;
        }
    /*
      Handle a window event.
    */
    timestamp=time((time_t *) NULL);
    (void) XNextEvent(display,&event);
    if (windows->image.stasis == MagickFalse)
      windows->image.stasis=(time((time_t *) NULL)-timestamp) > 0 ?
        MagickTrue : MagickFalse;
    if (event.xany.window == windows->command.id)
      {
        int
          id;

        /*
          Select a command from the Command widget.
        */
        id=XCommandWidget(display,windows,CommandMenu,&event);
        if (id < 0)
          continue;
        (void) CopyMagickString(command,CommandMenu[id],MaxTextExtent);
        command_type=CommandMenus[id];
        if (id < MagickMenus)
          {
            int
              entry;

            /*
              Select a command from a pop-up menu.
            */
            entry=XMenuWidget(display,windows,CommandMenu[id],Menus[id],
              command);
            if (entry < 0)
              continue;
            (void) CopyMagickString(command,Menus[id][entry],MaxTextExtent);
            command_type=Commands[id][entry];
          }
        if (command_type != NullCommand)
          nexus=XMagickCommand(display,resource_info,windows,
            command_type,&image,&state);
        continue;
      }
    switch (event.type)
    {
      case ButtonPress:
      {
        if (display_image->debug != MagickFalse)
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Button Press: 0x%lx %u +%d+%d",event.xbutton.window,
            event.xbutton.button,event.xbutton.x,event.xbutton.y);
        if ((event.xbutton.button == Button3) &&
            (event.xbutton.state & Mod1Mask))
          {
            /*
              Convert Alt-Button3 to Button2.
            */
            event.xbutton.button=Button2;
            event.xbutton.state&=(~Mod1Mask);
          }
        if (event.xbutton.window == windows->backdrop.id)
          {
            (void) XSetInputFocus(display,event.xbutton.window,RevertToParent,
              event.xbutton.time);
            break;
          }
        if (event.xbutton.window == windows->image.id)
          {
            if (resource_info->immutable != MagickFalse)
              {
                state|=ExitState;
                break;
              }
            /*
              Map/unmap Command widget.
            */
            if (windows->command.mapped)
              (void) XWithdrawWindow(display,windows->command.id,
                windows->command.screen);
            else
              {
                (void) XCommandWidget(display,windows,CommandMenu,
                  (XEvent *) NULL);
                (void) XMapRaised(display,windows->command.id);
              }
          }
        break;
      }
      case ButtonRelease:
      {
        if (display_image->debug != MagickFalse)
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Button Release: 0x%lx %u +%d+%d",event.xbutton.window,
            event.xbutton.button,event.xbutton.x,event.xbutton.y);
        break;
      }
      case ClientMessage:
      {
        if (display_image->debug != MagickFalse)
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Client Message: 0x%lx 0x%lx %d 0x%lx",(unsigned long)
            event.xclient.window,(unsigned long) event.xclient.message_type,
            event.xclient.format,(unsigned long) event.xclient.data.l[0]);
        if (event.xclient.message_type == windows->im_protocols)
          {
            if (*event.xclient.data.l == (long) windows->im_update_colormap)
              {
                /*
                  Update graphic context and window colormap.
                */
                for (i=0; i < (ssize_t) number_windows; i++)
                {
                  if (magick_windows[i]->id == windows->icon.id)
                    continue;
                  context_values.background=pixel->background_color.pixel;
                  context_values.foreground=pixel->foreground_color.pixel;
                  (void) XChangeGC(display,magick_windows[i]->annotate_context,
                    context_mask,&context_values);
                  (void) XChangeGC(display,magick_windows[i]->widget_context,
                    context_mask,&context_values);
                  context_values.background=pixel->foreground_color.pixel;
                  context_values.foreground=pixel->background_color.pixel;
                  context_values.plane_mask=
                    context_values.background ^ context_values.foreground;
                  (void) XChangeGC(display,magick_windows[i]->highlight_context,
                    (size_t) (context_mask | GCPlaneMask),
                    &context_values);
                  magick_windows[i]->attributes.background_pixel=
                    pixel->background_color.pixel;
                  magick_windows[i]->attributes.border_pixel=
                    pixel->border_color.pixel;
                  magick_windows[i]->attributes.colormap=map_info->colormap;
                  (void) XChangeWindowAttributes(display,magick_windows[i]->id,
                    (unsigned long) magick_windows[i]->mask,
                    &magick_windows[i]->attributes);
                }
                if (windows->backdrop.id != (Window) NULL)
                  (void) XInstallColormap(display,map_info->colormap);
                break;
              }
            if (*event.xclient.data.l == (long) windows->im_exit)
              {
                state|=ExitState;
                break;
              }
            break;
          }
        if (event.xclient.message_type == windows->dnd_protocols)
          {
            Atom
              selection,
              type;

            int
              format,
              status;

            unsigned char
              *data;

            unsigned long
              after,
              length;

            /*
              Display image named by the Drag-and-Drop selection.
            */
            if ((*event.xclient.data.l != 2) && (*event.xclient.data.l != 128))
              break;
            selection=XInternAtom(display,"DndSelection",MagickFalse);
            status=XGetWindowProperty(display,root_window,selection,0L,2047L,
              MagickFalse,(Atom) AnyPropertyType,&type,&format,&length,&after,
              &data);
            if ((status != Success) || (length == 0))
              break;
            if (*event.xclient.data.l == 2)
              {
                /*
                  Offix DND.
                */
                (void) CopyMagickString(resource_info->image_info->filename,
                  (char *) data,MaxTextExtent);
              }
            else
              {
                /*
                  XDND.
                */
                if (LocaleNCompare((char *) data,"file:",5) != 0)
                  {
                    (void) XFree((void *) data);
                    break;
                  }
                (void) CopyMagickString(resource_info->image_info->filename,
                  ((char *) data)+5,MaxTextExtent);
              }
            nexus=ReadImage(resource_info->image_info,&image->exception);
            CatchException(&image->exception);
            if (nexus != (Image *) NULL)
              state|=ExitState;
            (void) XFree((void *) data);
            break;
          }
        /*
          If client window delete message, exit.
        */
        if (event.xclient.message_type != windows->wm_protocols)
          break;
        if (*event.xclient.data.l == (long) windows->wm_take_focus)
          {
            (void) XSetInputFocus(display,event.xclient.window,RevertToParent,
              (Time) event.xclient.data.l[1]);
            break;
          }
        if (*event.xclient.data.l != (long) windows->wm_delete_window)
          break;
        (void) XWithdrawWindow(display,event.xclient.window,
          visual_info->screen);
        if (event.xclient.window == windows->image.id)
          {
            state|=ExitState;
            break;
          }
        break;
      }
      case ConfigureNotify:
      {
        if (display_image->debug != MagickFalse)
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Configure Notify: 0x%lx %dx%d+%d+%d %d",event.xconfigure.window,
            event.xconfigure.width,event.xconfigure.height,event.xconfigure.x,
            event.xconfigure.y,event.xconfigure.send_event);
        if (event.xconfigure.window == windows->image.id)
          {
            if (event.xconfigure.send_event != 0)
              {
                XWindowChanges
                  window_changes;

                /*
                  Position the transient windows relative of the Image window.
                */
                if (windows->command.geometry == (char *) NULL)
                  if (windows->command.mapped == MagickFalse)
                    {
                       windows->command.x=
                          event.xconfigure.x-windows->command.width-25;
                        windows->command.y=event.xconfigure.y;
                        XConstrainWindowPosition(display,&windows->command);
                        window_changes.x=windows->command.x;
                        window_changes.y=windows->command.y;
                        (void) XReconfigureWMWindow(display,windows->command.id,
                          windows->command.screen,(unsigned int) (CWX | CWY),
                          &window_changes);
                    }
                if (windows->widget.geometry == (char *) NULL)
                  if (windows->widget.mapped == MagickFalse)
                    {
                      windows->widget.x=
                        event.xconfigure.x+event.xconfigure.width/10;
                      windows->widget.y=
                        event.xconfigure.y+event.xconfigure.height/10;
                      XConstrainWindowPosition(display,&windows->widget);
                      window_changes.x=windows->widget.x;
                      window_changes.y=windows->widget.y;
                      (void) XReconfigureWMWindow(display,windows->widget.id,
                        windows->widget.screen,(unsigned int) (CWX | CWY),
                        &window_changes);
                    }
              }
            /*
              Image window has a new configuration.
            */
            windows->image.width=(unsigned int) event.xconfigure.width;
            windows->image.height=(unsigned int) event.xconfigure.height;
            break;
          }
        if (event.xconfigure.window == windows->icon.id)
          {
            /*
              Icon window has a new configuration.
            */
            windows->icon.width=(unsigned int) event.xconfigure.width;
            windows->icon.height=(unsigned int) event.xconfigure.height;
            break;
          }
        break;
      }
      case DestroyNotify:
      {
        /*
          Group leader has exited.
        */
        if (display_image->debug != MagickFalse)
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Destroy Notify: 0x%lx",event.xdestroywindow.window);
        if (event.xdestroywindow.window == windows->group_leader.id)
          {
            state|=ExitState;
            break;
          }
        break;
      }
      case EnterNotify:
      {
        /*
          Selectively install colormap.
        */
        if (map_info->colormap != XDefaultColormap(display,visual_info->screen))
          if (event.xcrossing.mode != NotifyUngrab)
            XInstallColormap(display,map_info->colormap);
        break;
      }
      case Expose:
      {
        if (display_image->debug != MagickFalse)
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Expose: 0x%lx %dx%d+%d+%d",event.xexpose.window,
            event.xexpose.width,event.xexpose.height,event.xexpose.x,
            event.xexpose.y);
        /*
          Repaint windows that are now exposed.
        */
        if (event.xexpose.window == windows->image.id)
          {
            windows->image.pixmap=windows->image.pixmaps[scene];
            windows->image.matte_pixmap=windows->image.matte_pixmaps[scene];
            XRefreshWindow(display,&windows->image,&event);
            break;
          }
        if (event.xexpose.window == windows->icon.id)
          if (event.xexpose.count == 0)
            {
              XRefreshWindow(display,&windows->icon,&event);
              break;
            }
        break;
      }
      case KeyPress:
      {
        static int
          length;

        /*
          Respond to a user key press.
        */
        length=XLookupString((XKeyEvent *) &event.xkey,command,(int)
          sizeof(command),&key_symbol,(XComposeStatus *) NULL);
        *(command+length)='\0';
        if (display_image->debug != MagickFalse)
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Key press: 0x%lx (%c)",(unsigned long) key_symbol,*command);
        command_type=NullCommand;
        switch (key_symbol)
        {
          case XK_o:
          {
            if ((event.xkey.state & ControlMask) == MagickFalse)
              break;
            command_type=OpenCommand;
            break;
          }
          case XK_BackSpace:
          {
            command_type=StepBackwardCommand;
            break;
          }
          case XK_space:
          {
            command_type=StepForwardCommand;
            break;
          }
          case XK_less:
          {
            command_type=FasterCommand;
            break;
          }
          case XK_greater:
          {
            command_type=SlowerCommand;
            break;
          }
          case XK_F1:
          {
            command_type=HelpCommand;
            break;
          }
          case XK_Find:
          {
            command_type=BrowseDocumentationCommand;
            break;
          }
          case XK_question:
          {
            command_type=InfoCommand;
            break;
          }
          case XK_q:
          case XK_Escape:
          {
            command_type=QuitCommand;
            break;
          }
          default:
            break;
        }
        if (command_type != NullCommand)
          nexus=XMagickCommand(display,resource_info,windows,
            command_type,&image,&state);
        break;
      }
      case KeyRelease:
      {
        /*
          Respond to a user key release.
        */
        (void) XLookupString((XKeyEvent *) &event.xkey,command,(int)
          sizeof(command),&key_symbol,(XComposeStatus *) NULL);
        if (display_image->debug != MagickFalse)
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Key release: 0x%lx (%c)",(unsigned long) key_symbol,*command);
        break;
      }
      case LeaveNotify:
      {
        /*
          Selectively uninstall colormap.
        */
        if (map_info->colormap != XDefaultColormap(display,visual_info->screen))
          if (event.xcrossing.mode != NotifyUngrab)
            XUninstallColormap(display,map_info->colormap);
        break;
      }
      case MapNotify:
      {
        if (display_image->debug != MagickFalse)
          (void) LogMagickEvent(X11Event,GetMagickModule(),"Map Notify: 0x%lx",
            event.xmap.window);
        if (event.xmap.window == windows->backdrop.id)
          {
            (void) XSetInputFocus(display,event.xmap.window,RevertToParent,
              CurrentTime);
            windows->backdrop.mapped=MagickTrue;
            break;
          }
        if (event.xmap.window == windows->image.id)
          {
            if (windows->backdrop.id != (Window) NULL)
              (void) XInstallColormap(display,map_info->colormap);
            if (LocaleCompare(image_list[0]->magick,"LOGO") == 0)
              {
                if (LocaleCompare(display_image->filename,"LOGO") == 0)
                  nexus=XMagickCommand(display,resource_info,windows,
                    OpenCommand,&image,&state);
                else
                  state|=ExitState;
              }
            windows->image.mapped=MagickTrue;
            break;
          }
        if (event.xmap.window == windows->info.id)
          {
            windows->info.mapped=MagickTrue;
            break;
          }
        if (event.xmap.window == windows->icon.id)
          {
            /*
              Create an icon image.
            */
            XMakeStandardColormap(display,icon_visual,icon_resources,
              display_image,icon_map,icon_pixel);
            (void) XMakeImage(display,icon_resources,&windows->icon,
              display_image,windows->icon.width,windows->icon.height);
            (void) XSetWindowBackgroundPixmap(display,windows->icon.id,
              windows->icon.pixmap);
            (void) XClearWindow(display,windows->icon.id);
            (void) XWithdrawWindow(display,windows->info.id,
              windows->info.screen);
            windows->icon.mapped=MagickTrue;
            break;
          }
        if (event.xmap.window == windows->command.id)
          {
            windows->command.mapped=MagickTrue;
            break;
          }
        if (event.xmap.window == windows->popup.id)
          {
            windows->popup.mapped=MagickTrue;
            break;
          }
        if (event.xmap.window == windows->widget.id)
          {
            windows->widget.mapped=MagickTrue;
            break;
          }
        break;
      }
      case MappingNotify:
      {
        (void) XRefreshKeyboardMapping(&event.xmapping);
        break;
      }
      case NoExpose:
        break;
      case PropertyNotify:
      {
        Atom
          type;

        int
          format,
          status;

        unsigned char
          *data;

        unsigned long
          after,
          length;

        if (display_image->debug != MagickFalse)
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Property Notify: 0x%lx 0x%lx %d",(unsigned long)
            event.xproperty.window,(unsigned long) event.xproperty.atom,
            event.xproperty.state);
        if (event.xproperty.atom != windows->im_remote_command)
          break;
        /*
          Display image named by the remote command protocol.
        */
        status=XGetWindowProperty(display,event.xproperty.window,
          event.xproperty.atom,0L,(long) MaxTextExtent,MagickFalse,(Atom)
          AnyPropertyType,&type,&format,&length,&after,&data);
        if ((status != Success) || (length == 0))
          break;
        (void) CopyMagickString(resource_info->image_info->filename,
          (char *) data,MaxTextExtent);
        nexus=ReadImage(resource_info->image_info,&image->exception);
        CatchException(&image->exception);
        if (nexus != (Image *) NULL)
          state|=ExitState;
        (void) XFree((void *) data);
        break;
      }
      case ReparentNotify:
      {
        if (display_image->debug != MagickFalse)
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Reparent Notify: 0x%lx=>0x%lx",event.xreparent.parent,
            event.xreparent.window);
        break;
      }
      case UnmapNotify:
      {
        if (display_image->debug != MagickFalse)
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Unmap Notify: 0x%lx",event.xunmap.window);
        if (event.xunmap.window == windows->backdrop.id)
          {
            windows->backdrop.mapped=MagickFalse;
            break;
          }
        if (event.xunmap.window == windows->image.id)
          {
            windows->image.mapped=MagickFalse;
            break;
          }
        if (event.xunmap.window == windows->info.id)
          {
            windows->info.mapped=MagickFalse;
            break;
          }
        if (event.xunmap.window == windows->icon.id)
          {
            if (map_info->colormap == icon_map->colormap)
              XConfigureImageColormap(display,resource_info,windows,
                display_image);
            (void) XFreeStandardColormap(display,icon_visual,icon_map,
              icon_pixel);
            windows->icon.mapped=MagickFalse;
            break;
          }
        if (event.xunmap.window == windows->command.id)
          {
            windows->command.mapped=MagickFalse;
            break;
          }
        if (event.xunmap.window == windows->popup.id)
          {
            if (windows->backdrop.id != (Window) NULL)
              (void) XSetInputFocus(display,windows->image.id,RevertToParent,
                CurrentTime);
            windows->popup.mapped=MagickFalse;
            break;
          }
        if (event.xunmap.window == windows->widget.id)
          {
            if (windows->backdrop.id != (Window) NULL)
              (void) XSetInputFocus(display,windows->image.id,RevertToParent,
                CurrentTime);
            windows->widget.mapped=MagickFalse;
            break;
          }
        break;
      }
      default:
      {
        if (display_image->debug != MagickFalse)
          (void) LogMagickEvent(X11Event,GetMagickModule(),"Event type: %d",
            event.type);
        break;
      }
    }
  }
  while (!(state & ExitState));
  image_list=(Image **) RelinquishMagickMemory(image_list);
  images=DestroyImageList(images);
  if ((windows->visual_info->klass == GrayScale) ||
      (windows->visual_info->klass == PseudoColor) ||
      (windows->visual_info->klass == DirectColor))
    {
      /*
        Withdraw windows.
      */
      if (windows->info.mapped)
        (void) XWithdrawWindow(display,windows->info.id,windows->info.screen);
      if (windows->command.mapped)
        (void) XWithdrawWindow(display,windows->command.id,
          windows->command.screen);
    }
  if (resource_info->backdrop == MagickFalse)
    if (windows->backdrop.mapped)
      {
        (void) XWithdrawWindow(display,windows->backdrop.id,\
          windows->backdrop.screen);
        (void) XDestroyWindow(display,windows->backdrop.id);
        windows->backdrop.id=(Window) NULL;
        (void) XWithdrawWindow(display,windows->image.id,windows->image.screen);
        (void) XDestroyWindow(display,windows->image.id);
        windows->image.id=(Window) NULL;
      }
  XSetCursorState(display,windows,MagickTrue);
  XCheckRefreshWindows(display,windows);
  for (scene=1; scene < (ssize_t) number_scenes; scene++)
  {
    if (windows->image.pixmaps[scene] != (Pixmap) NULL)
      (void) XFreePixmap(display,windows->image.pixmaps[scene]);
    windows->image.pixmaps[scene]=(Pixmap) NULL;
    if (windows->image.matte_pixmaps[scene] != (Pixmap) NULL)
      (void) XFreePixmap(display,windows->image.matte_pixmaps[scene]);
    windows->image.matte_pixmaps[scene]=(Pixmap) NULL;
  }
  XSetCursorState(display,windows,MagickFalse);
  windows->image.pixmaps=(Pixmap *)
    RelinquishMagickMemory(windows->image.pixmaps);
  windows->image.matte_pixmaps=(Pixmap *)
    RelinquishMagickMemory(windows->image.matte_pixmaps);
  if (nexus == (Image *) NULL)
    {
      /*
        Free X resources.
      */
      if (windows->image.mapped != MagickFalse)
        (void) XWithdrawWindow(display,windows->image.id,windows->image.screen);
      XDelay(display,SuspendTime);
      (void) XFreeStandardColormap(display,icon_visual,icon_map,icon_pixel);
      if (resource_info->map_type == (char *) NULL)
        (void) XFreeStandardColormap(display,visual_info,map_info,pixel);
      DestroyXResources();
    }
  (void) XSync(display,MagickFalse);
  /*
    Restore our progress monitor and warning handlers.
  */
  (void) SetErrorHandler(warning_handler);
  (void) SetWarningHandler(warning_handler);
  /*
    Change to home directory.
  */
  directory=getcwd(working_directory,MaxTextExtent);
  (void) directory;
  if (*resource_info->home_directory == '\0')
    (void) CopyMagickString(resource_info->home_directory,".",MaxTextExtent);
  status=chdir(resource_info->home_directory);
  if (status == -1)
    (void) ThrowMagickException(&images->exception,GetMagickModule(),
      FileOpenError,"UnableToOpenFile","%s",resource_info->home_directory);
  return(nexus);
}