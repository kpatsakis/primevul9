static Image *XGetWindowImage(Display *display,const Window window,
  const unsigned int borders,const unsigned int level,ExceptionInfo *exception)
{
  typedef struct _ColormapInfo
  {
    Colormap
      colormap;

    XColor
      *colors;

    struct _ColormapInfo
      *next;
  } ColormapInfo;

  typedef struct _WindowInfo
  {
    Window
      window,
      parent;

    Visual
      *visual;

    Colormap
      colormap;

    XSegment
      bounds;

    RectangleInfo
      crop_info;
  } WindowInfo;

  int
    display_height,
    display_width,
    id,
    x_offset,
    y_offset;

  Quantum
    index;

  RectangleInfo
    crop_info;

  register int
    i;

  static ColormapInfo
    *colormap_info = (ColormapInfo *) NULL;

  static int
    max_windows = 0,
    number_windows = 0;

  static WindowInfo
    *window_info;

  Status
    status;

  Window
    child,
    root_window;

  XWindowAttributes
    window_attributes;

  /*
    Verify window is viewable.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  status=XGetWindowAttributes(display,window,&window_attributes);
  if ((status == False) || (window_attributes.map_state != IsViewable))
    return((Image *) NULL);
  /*
    Cropping rectangle is relative to root window.
  */
  root_window=XRootWindow(display,XDefaultScreen(display));
  (void) XTranslateCoordinates(display,window,root_window,0,0,&x_offset,
    &y_offset,&child);
  crop_info.x=(ssize_t) x_offset;
  crop_info.y=(ssize_t) y_offset;
  crop_info.width=(size_t) window_attributes.width;
  crop_info.height=(size_t) window_attributes.height;
  if (borders != MagickFalse)
    {
      /*
        Include border in image.
      */
      crop_info.x-=(ssize_t) window_attributes.border_width;
      crop_info.y-=(ssize_t) window_attributes.border_width;
      crop_info.width+=(size_t) (window_attributes.border_width << 1);
      crop_info.height+=(size_t) (window_attributes.border_width << 1);
    }
  /*
    Crop to root window.
  */
  if (crop_info.x < 0)
    {
      crop_info.width+=crop_info.x;
      crop_info.x=0;
    }
  if (crop_info.y < 0)
    {
      crop_info.height+=crop_info.y;
      crop_info.y=0;
    }
  display_width=XDisplayWidth(display,XDefaultScreen(display));
  if ((int) (crop_info.x+crop_info.width) > display_width)
    crop_info.width=(size_t) (display_width-crop_info.x);
  display_height=XDisplayHeight(display,XDefaultScreen(display));
  if ((int) (crop_info.y+crop_info.height) > display_height)
    crop_info.height=(size_t) (display_height-crop_info.y);
  /*
    Initialize window info attributes.
  */
  if (number_windows >= max_windows)
    {
      /*
        Allocate or resize window info buffer.
      */
      max_windows+=1024;
      if (window_info == (WindowInfo *) NULL)
        window_info=(WindowInfo *) AcquireQuantumMemory((size_t) max_windows,
          sizeof(*window_info));
      else
        window_info=(WindowInfo *) ResizeQuantumMemory(window_info,(size_t)
          max_windows,sizeof(*window_info));
    }
  if (window_info == (WindowInfo *) NULL)
    {
      ThrowXWindowException(ResourceLimitError,"MemoryAllocationFailed","...");
      return((Image *) NULL);
    }
  id=number_windows++;
  window_info[id].window=window;
  window_info[id].visual=window_attributes.visual;
  window_info[id].colormap=window_attributes.colormap;
  window_info[id].bounds.x1=(short) crop_info.x;
  window_info[id].bounds.y1=(short) crop_info.y;
  window_info[id].bounds.x2=(short) (crop_info.x+(int) crop_info.width-1);
  window_info[id].bounds.y2=(short) (crop_info.y+(int) crop_info.height-1);
  crop_info.x-=x_offset;
  crop_info.y-=y_offset;
  window_info[id].crop_info=crop_info;
  if (level != 0)
    {
      unsigned int
        number_children;

      Window
        *children;

      /*
        Descend the window hierarchy.
      */
      status=XQueryTree(display,window,&root_window,&window_info[id].parent,
        &children,&number_children);
      for (i=0; i < id; i++)
        if ((window_info[i].window == window_info[id].parent) &&
            (window_info[i].visual == window_info[id].visual) &&
            (window_info[i].colormap == window_info[id].colormap))
          {
            if ((window_info[id].bounds.x1 < window_info[i].bounds.x1) ||
                (window_info[id].bounds.x2 > window_info[i].bounds.x2) ||
                (window_info[id].bounds.y1 < window_info[i].bounds.y1) ||
                (window_info[id].bounds.y2 > window_info[i].bounds.y2))
              {
                /*
                  Eliminate windows not circumscribed by their parent.
                */
                number_windows--;
                break;
              }
          }
      if ((status == True) && (number_children != 0))
        {
          for (i=0; i < (int) number_children; i++)
            (void) XGetWindowImage(display,children[i],MagickFalse,level+1,
              exception);
          (void) XFree((void *) children);
        }
    }
  if (level <= 1)
    {
      CacheView
        *composite_view;

      ColormapInfo
        *next;

      Image
        *composite_image,
        *image;

      int
        y;

      MagickBooleanType
        import;

      register int
        j,
        x;

      register Quantum
        *magick_restrict q;

      register size_t
        pixel;

      unsigned int
        number_colors;

      XColor
        *colors;

      XImage
        *ximage;

      /*
        Get X image for each window in the list.
      */
      image=NewImageList();
      for (id=0; id < number_windows; id++)
      {
        /*
          Does target window intersect top level window?
        */
        import=((window_info[id].bounds.x2 >= window_info[0].bounds.x1) &&
           (window_info[id].bounds.x1 <= window_info[0].bounds.x2) &&
           (window_info[id].bounds.y2 >= window_info[0].bounds.y1) &&
           (window_info[id].bounds.y1 <= window_info[0].bounds.y2)) ?
          MagickTrue : MagickFalse;
        /*
          Is target window contained by another window with the same colormap?
        */
        for (j=0; j < id; j++)
          if ((window_info[id].visual == window_info[j].visual) &&
              (window_info[id].colormap == window_info[j].colormap))
            {
              if ((window_info[id].bounds.x1 >= window_info[j].bounds.x1) &&
                  (window_info[id].bounds.x2 <= window_info[j].bounds.x2) &&
                  (window_info[id].bounds.y1 >= window_info[j].bounds.y1) &&
                  (window_info[id].bounds.y2 <= window_info[j].bounds.y2))
                import=MagickFalse;
            }
        if (import == MagickFalse)
          continue;
        /*
          Get X image.
        */
        ximage=XGetImage(display,window_info[id].window,(int)
          window_info[id].crop_info.x,(int) window_info[id].crop_info.y,
          (unsigned int) window_info[id].crop_info.width,(unsigned int)
          window_info[id].crop_info.height,AllPlanes,ZPixmap);
        if (ximage == (XImage *) NULL)
          continue;
        /*
          Initialize window colormap.
        */
        number_colors=0;
        colors=(XColor *) NULL;
        if (window_info[id].colormap != (Colormap) NULL)
          {
            ColormapInfo
              *p;

            /*
              Search colormap list for window colormap.
            */
            number_colors=(unsigned int) window_info[id].visual->map_entries;
            for (p=colormap_info; p != (ColormapInfo *) NULL; p=p->next)
              if (p->colormap == window_info[id].colormap)
                break;
            if (p == (ColormapInfo *) NULL)
              {
                /*
                  Get the window colormap.
                */
                colors=(XColor *) AcquireQuantumMemory(number_colors,
                  sizeof(*colors));
                if (colors == (XColor *) NULL)
                  {
                    XDestroyImage(ximage);
                    return((Image *) NULL);
                  }
                if ((window_info[id].visual->klass != DirectColor) &&
                    (window_info[id].visual->klass != TrueColor))
                  for (i=0; i < (int) number_colors; i++)
                  {
                    colors[i].pixel=(size_t) i;
                    colors[i].pad='\0';
                  }
                else
                  {
                    size_t
                      blue,
                      blue_bit,
                      green,
                      green_bit,
                      red,
                      red_bit;

                    /*
                      DirectColor or TrueColor visual.
                    */
                    red=0;
                    green=0;
                    blue=0;
                    red_bit=window_info[id].visual->red_mask &
                      (~(window_info[id].visual->red_mask)+1);
                    green_bit=window_info[id].visual->green_mask &
                      (~(window_info[id].visual->green_mask)+1);
                    blue_bit=window_info[id].visual->blue_mask &
                      (~(window_info[id].visual->blue_mask)+1);
                    for (i=0; i < (int) number_colors; i++)
                    {
                      colors[i].pixel=(unsigned long) (red | green | blue);
                      colors[i].pad='\0';
                      red+=red_bit;
                      if (red > window_info[id].visual->red_mask)
                        red=0;
                      green+=green_bit;
                      if (green > window_info[id].visual->green_mask)
                        green=0;
                      blue+=blue_bit;
                      if (blue > window_info[id].visual->blue_mask)
                        blue=0;
                    }
                  }
                (void) XQueryColors(display,window_info[id].colormap,colors,
                  (int) number_colors);
                /*
                  Append colormap to colormap list.
                */
                p=(ColormapInfo *) AcquireMagickMemory(sizeof(*p));
                if (p == (ColormapInfo *) NULL)
                  return((Image *) NULL);
                p->colormap=window_info[id].colormap;
                p->colors=colors;
                p->next=colormap_info;
                colormap_info=p;
              }
            colors=p->colors;
          }
        /*
          Allocate image structure.
        */
        composite_image=AcquireImage((ImageInfo *) NULL,exception);
        if (composite_image == (Image *) NULL)
          {
            XDestroyImage(ximage);
            return((Image *) NULL);
          }
        /*
          Convert X image to MIFF format.
        */
        if ((window_info[id].visual->klass != TrueColor) &&
            (window_info[id].visual->klass != DirectColor))
          composite_image->storage_class=PseudoClass;
        composite_image->columns=(size_t) ximage->width;
        composite_image->rows=(size_t) ximage->height;
        composite_view=AcquireAuthenticCacheView(composite_image,exception);
        switch (composite_image->storage_class)
        {
          case DirectClass:
          default:
          {
            register size_t
              color,
              index;

            size_t
              blue_mask,
              blue_shift,
              green_mask,
              green_shift,
              red_mask,
              red_shift;

            /*
              Determine shift and mask for red, green, and blue.
            */
            red_mask=window_info[id].visual->red_mask;
            red_shift=0;
            while ((red_mask != 0) && ((red_mask & 0x01) == 0))
            {
              red_mask>>=1;
              red_shift++;
            }
            green_mask=window_info[id].visual->green_mask;
            green_shift=0;
            while ((green_mask != 0) && ((green_mask & 0x01) == 0))
            {
              green_mask>>=1;
              green_shift++;
            }
            blue_mask=window_info[id].visual->blue_mask;
            blue_shift=0;
            while ((blue_mask != 0) && ((blue_mask & 0x01) == 0))
            {
              blue_mask>>=1;
              blue_shift++;
            }
            /*
              Convert X image to DirectClass packets.
            */
            if ((number_colors != 0) &&
                (window_info[id].visual->klass == DirectColor))
              for (y=0; y < (int) composite_image->rows; y++)
              {
                q=QueueCacheViewAuthenticPixels(composite_view,0,(ssize_t) y,
                  composite_image->columns,1,exception);
                if (q == (Quantum *) NULL)
                  break;
                for (x=0; x < (int) composite_image->columns; x++)
                {
                  pixel=XGetPixel(ximage,x,y);
                  index=(pixel >> red_shift) & red_mask;
                  SetPixelRed(composite_image,
                    ScaleShortToQuantum(colors[index].red),q);
                  index=(pixel >> green_shift) & green_mask;
                  SetPixelGreen(composite_image,
                    ScaleShortToQuantum(colors[index].green),q);
                  index=(pixel >> blue_shift) & blue_mask;
                  SetPixelBlue(composite_image,
                    ScaleShortToQuantum(colors[index].blue),q);
                  q+=GetPixelChannels(composite_image);
                }
                status=SyncCacheViewAuthenticPixels(composite_view,exception);
                if (status == MagickFalse)
                  break;
              }
            else
              for (y=0; y < (int) composite_image->rows; y++)
              {
                q=QueueCacheViewAuthenticPixels(composite_view,0,(ssize_t) y,
                  composite_image->columns,1,exception);
                if (q == (Quantum *) NULL)
                  break;
                for (x=0; x < (int) composite_image->columns; x++)
                {
                  pixel=XGetPixel(ximage,x,y);
                  color=(pixel >> red_shift) & red_mask;
                  if (red_mask != 0)
                    color=(65535UL*color)/red_mask;
                  SetPixelRed(composite_image,ScaleShortToQuantum(
                    (unsigned short) color),q);
                  color=(pixel >> green_shift) & green_mask;
                  if (green_mask != 0)
                    color=(65535UL*color)/green_mask;
                  SetPixelGreen(composite_image,ScaleShortToQuantum(
                    (unsigned short) color),q);
                  color=(pixel >> blue_shift) & blue_mask;
                  if (blue_mask != 0)
                    color=(65535UL*color)/blue_mask;
                  SetPixelBlue(composite_image,ScaleShortToQuantum(
                    (unsigned short) color),q);
                  q+=GetPixelChannels(composite_image);
                }
                status=SyncCacheViewAuthenticPixels(composite_view,exception);
                if (status == MagickFalse)
                  break;
              }
            break;
          }
          case PseudoClass:
          {
            /*
              Create colormap.
            */
            status=AcquireImageColormap(composite_image,number_colors,
              exception);
            if (status == MagickFalse)
              {
                XDestroyImage(ximage);
                composite_image=DestroyImage(composite_image);
                return((Image *) NULL);
              }
            for (i=0; i < (int) composite_image->colors; i++)
            {
              composite_image->colormap[colors[i].pixel].red=(double)
                ScaleShortToQuantum(colors[i].red);
              composite_image->colormap[colors[i].pixel].green=(double)
                ScaleShortToQuantum(colors[i].green);
              composite_image->colormap[colors[i].pixel].blue=(double)
                ScaleShortToQuantum(colors[i].blue);
            }
            /*
              Convert X image to PseudoClass packets.
            */
            for (y=0; y < (int) composite_image->rows; y++)
            {
              q=QueueCacheViewAuthenticPixels(composite_view,0,(ssize_t) y,
                composite_image->columns,1,exception);
              if (q == (Quantum *) NULL)
                break;
              for (x=0; x < (int) composite_image->columns; x++)
              {
                index=(Quantum) XGetPixel(ximage,x,y);
                SetPixelIndex(composite_image,index,q);
                SetPixelViaPixelInfo(composite_image,
                  composite_image->colormap+(ssize_t) index,q);
                q+=GetPixelChannels(composite_image);
              }
              status=SyncCacheViewAuthenticPixels(composite_view,exception);
              if (status == MagickFalse)
                break;
            }
            break;
          }
        }
        composite_view=DestroyCacheView(composite_view);
        XDestroyImage(ximage);
        if (image == (Image *) NULL)
          {
            image=composite_image;
            continue;
          }
        /*
          Composite any children in back-to-front order.
        */
        (void) XTranslateCoordinates(display,window_info[id].window,window,0,0,
          &x_offset,&y_offset,&child);
        x_offset-=(int) crop_info.x;
        if (x_offset < 0)
          x_offset=0;
        y_offset-=(int) crop_info.y;
        if (y_offset < 0)
          y_offset=0;
        (void) CompositeImage(image,composite_image,CopyCompositeOp,MagickTrue,
          (ssize_t) x_offset,(ssize_t) y_offset,exception);
        composite_image=DestroyImage(composite_image);
      }
      /*
        Relinquish resources.
      */
      while (colormap_info != (ColormapInfo *) NULL)
      {
        next=colormap_info->next;
        colormap_info->colors=(XColor *) RelinquishMagickMemory(
          colormap_info->colors);
        colormap_info=(ColormapInfo *) RelinquishMagickMemory(colormap_info);
        colormap_info=next;
      }
      /*
        Relinquish resources and restore initial state.
      */
      window_info=(WindowInfo *) RelinquishMagickMemory(window_info);
      max_windows=0;
      number_windows=0;
      colormap_info=(ColormapInfo *) NULL;
      return(image);
    }
  return((Image *) NULL);
}