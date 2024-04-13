MagickPrivate XVisualInfo *XBestVisualInfo(Display *display,
  XStandardColormap *map_info,XResourceInfo *resource_info)
{
#define MaxStandardColormaps  7
#define XVisualColormapSize(visual_info) MagickMin((unsigned int) (\
  (visual_info->klass == TrueColor) || (visual_info->klass == DirectColor) ? \
   visual_info->red_mask | visual_info->green_mask | visual_info->blue_mask : \
   (unsigned long) visual_info->colormap_size),1UL << visual_info->depth)

  char
    *map_type,
    *visual_type;

  int
    visual_mask;

  register int
    i;

  size_t
    one;

  static int
    number_visuals;

  static XVisualInfo
    visual_template;

  XVisualInfo
    *visual_info,
    *visual_list;

  /*
    Restrict visual search by screen number.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(map_info != (XStandardColormap *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  map_type=resource_info->map_type;
  visual_type=resource_info->visual_type;
  visual_mask=VisualScreenMask;
  visual_template.screen=XDefaultScreen(display);
  visual_template.depth=XDefaultDepth(display,XDefaultScreen(display));
  one=1;
  if ((resource_info->immutable != MagickFalse) && (resource_info->colors != 0))
    if (resource_info->colors <= (one << (size_t) visual_template.depth))
      visual_mask|=VisualDepthMask;
  if (visual_type != (char *) NULL)
    {
      /*
        Restrict visual search by class or visual id.
      */
      if (LocaleCompare("staticgray",visual_type) == 0)
        {
          visual_mask|=VisualClassMask;
          visual_template.klass=StaticGray;
        }
      else
        if (LocaleCompare("grayscale",visual_type) == 0)
          {
            visual_mask|=VisualClassMask;
            visual_template.klass=GrayScale;
          }
        else
          if (LocaleCompare("staticcolor",visual_type) == 0)
            {
              visual_mask|=VisualClassMask;
              visual_template.klass=StaticColor;
            }
          else
            if (LocaleCompare("pseudocolor",visual_type) == 0)
              {
                visual_mask|=VisualClassMask;
                visual_template.klass=PseudoColor;
              }
            else
              if (LocaleCompare("truecolor",visual_type) == 0)
                {
                  visual_mask|=VisualClassMask;
                  visual_template.klass=TrueColor;
                }
              else
                if (LocaleCompare("directcolor",visual_type) == 0)
                  {
                    visual_mask|=VisualClassMask;
                    visual_template.klass=DirectColor;
                  }
                else
                  if (LocaleCompare("default",visual_type) == 0)
                    {
                      visual_mask|=VisualIDMask;
                      visual_template.visualid=XVisualIDFromVisual(
                        XDefaultVisual(display,XDefaultScreen(display)));
                    }
                  else
                    if (isdigit((int) ((unsigned char) *visual_type)) != 0)
                      {
                        visual_mask|=VisualIDMask;
                        visual_template.visualid=
                          strtol(visual_type,(char **) NULL,0);
                      }
                    else
                      ThrowXWindowException(XServerError,
                        "UnrecognizedVisualSpecifier",visual_type);
    }
  /*
    Get all visuals that meet our criteria so far.
  */
  number_visuals=0;
  visual_list=XGetVisualInfo(display,visual_mask,&visual_template,
    &number_visuals);
  visual_mask=VisualScreenMask | VisualIDMask;
  if ((number_visuals == 0) || (visual_list == (XVisualInfo *) NULL))
    {
      /*
        Failed to get visual;  try using the default visual.
      */
      ThrowXWindowException(XServerWarning,"UnableToGetVisual",visual_type);
      visual_template.visualid=XVisualIDFromVisual(XDefaultVisual(display,
        XDefaultScreen(display)));
      visual_list=XGetVisualInfo(display,visual_mask,&visual_template,
        &number_visuals);
      if ((number_visuals == 0) || (visual_list == (XVisualInfo *) NULL))
        return((XVisualInfo *) NULL);
      ThrowXWindowException(XServerWarning,"UsingDefaultVisual",
        XVisualClassName(visual_list->klass));
    }
  resource_info->color_recovery=MagickFalse;
  if ((map_info != (XStandardColormap *) NULL) && (map_type != (char *) NULL))
    {
      Atom
        map_property;

      char
        map_name[MagickPathExtent];

      int
        j,
        number_maps;

      Status
        status;

      Window
        root_window;

      XStandardColormap
        *map_list;

      /*
        Choose a visual associated with a standard colormap.
      */
      root_window=XRootWindow(display,XDefaultScreen(display));
      status=False;
      number_maps=0;
      if (LocaleCompare(map_type,"list") != 0)
        {
          /*
            User specified Standard Colormap.
          */
          (void) FormatLocaleString((char *) map_name,MagickPathExtent,
            "RGB_%s_MAP",map_type);
          LocaleUpper(map_name);
          map_property=XInternAtom(display,(char *) map_name,MagickTrue);
          if (map_property != (Atom) NULL)
            status=XGetRGBColormaps(display,root_window,&map_list,&number_maps,
              map_property);
        }
      else
        {
          static const char
            *colormap[MaxStandardColormaps]=
            {
              "_HP_RGB_SMOOTH_MAP_LIST",
              "RGB_BEST_MAP",
              "RGB_DEFAULT_MAP",
              "RGB_GRAY_MAP",
              "RGB_RED_MAP",
              "RGB_GREEN_MAP",
              "RGB_BLUE_MAP",
            };

          /*
            Choose a standard colormap from a list.
          */
          for (i=0; i < MaxStandardColormaps; i++)
          {
            map_property=XInternAtom(display,(char *) colormap[i],MagickTrue);
            if (map_property == (Atom) NULL)
              continue;
            status=XGetRGBColormaps(display,root_window,&map_list,&number_maps,
              map_property);
            if (status != False)
              break;
          }
          resource_info->color_recovery=i == 0 ? MagickTrue : MagickFalse;
        }
      if (status == False)
        {
          ThrowXWindowException(XServerError,"UnableToGetStandardColormap",
            map_type);
          return((XVisualInfo *) NULL);
        }
      /*
        Search all Standard Colormaps and visuals for ids that match.
      */
      *map_info=map_list[0];
#if !defined(PRE_R4_ICCCM)
      visual_template.visualid=XVisualIDFromVisual(visual_list[0].visual);
      for (i=0; i < number_maps; i++)
        for (j=0; j < number_visuals; j++)
          if (map_list[i].visualid ==
              XVisualIDFromVisual(visual_list[j].visual))
            {
              *map_info=map_list[i];
              visual_template.visualid=XVisualIDFromVisual(
                visual_list[j].visual);
              break;
            }
      if (map_info->visualid != visual_template.visualid)
        {
          ThrowXWindowException(XServerError,
            "UnableToMatchVisualToStandardColormap",map_type);
          return((XVisualInfo *) NULL);
        }
#endif
      if (map_info->colormap == (Colormap) NULL)
        {
          ThrowXWindowException(XServerError,"StandardColormapIsNotInitialized",
            map_type);
          return((XVisualInfo *) NULL);
        }
      (void) XFree((void *) map_list);
    }
  else
    {
      static const unsigned int
        rank[]=
          {
            StaticGray,
            GrayScale,
            StaticColor,
            DirectColor,
            TrueColor,
            PseudoColor
          };

      XVisualInfo
        *p;

      /*
        Pick one visual that displays the most simultaneous colors.
      */
      visual_info=visual_list;
      p=visual_list;
      for (i=1; i < number_visuals; i++)
      {
        p++;
        if (XVisualColormapSize(p) > XVisualColormapSize(visual_info))
          visual_info=p;
        else
          if (XVisualColormapSize(p) == XVisualColormapSize(visual_info))
            if (rank[p->klass] > rank[visual_info->klass])
              visual_info=p;
      }
      visual_template.visualid=XVisualIDFromVisual(visual_info->visual);
    }
  (void) XFree((void *) visual_list);
  /*
    Retrieve only one visual by its screen & id number.
  */
  visual_info=XGetVisualInfo(display,visual_mask,&visual_template,
    &number_visuals);
  if ((number_visuals == 0) || (visual_info == (XVisualInfo *) NULL))
    return((XVisualInfo *) NULL);
  return(visual_info);
}