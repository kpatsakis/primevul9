MagickPrivate void XMakeStandardColormap(Display *display,
  XVisualInfo *visual_info,XResourceInfo *resource_info,Image *image,
  XStandardColormap *map_info,XPixelInfo *pixel,ExceptionInfo *exception)
{
  Colormap
    colormap;

  register ssize_t
    i;

  Status
    status;

  size_t
    number_colors,
    retain_colors;

  unsigned short
    gray_value;

  XColor
    color,
    *colors,
    *p;

  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(visual_info != (XVisualInfo *) NULL);
  assert(map_info != (XStandardColormap *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  assert(pixel != (XPixelInfo *) NULL);
  if (resource_info->map_type != (char *) NULL)
    {
      /*
        Standard Colormap is already defined (i.e. xstdcmap).
      */
      XGetPixelInfo(display,visual_info,map_info,resource_info,image,
        pixel);
      number_colors=(unsigned int) (map_info->base_pixel+
        (map_info->red_max+1)*(map_info->green_max+1)*(map_info->blue_max+1));
      if ((map_info->red_max*map_info->green_max*map_info->blue_max) != 0)
        if ((image->alpha_trait == UndefinedPixelTrait) &&
            (resource_info->color_recovery == MagickFalse) &&
            (resource_info->quantize_info->dither_method != NoDitherMethod) &&
            (number_colors < MaxColormapSize))
          {
            Image
              *affinity_image;

            register Quantum
              *magick_restrict q;

            /*
              Improve image appearance with error diffusion.
            */
            affinity_image=AcquireImage((ImageInfo *) NULL,exception);
            if (affinity_image == (Image *) NULL)
              ThrowXWindowFatalException(ResourceLimitFatalError,
                "UnableToDitherImage",image->filename);
            affinity_image->columns=number_colors;
            affinity_image->rows=1;
            /*
              Initialize colormap image.
            */
            q=QueueAuthenticPixels(affinity_image,0,0,affinity_image->columns,
              1,exception);
            if (q != (Quantum *) NULL)
              {
                for (i=0; i < (ssize_t) number_colors; i++)
                {
                  SetPixelRed(affinity_image,0,q);
                  if (map_info->red_max != 0)
                    SetPixelRed(affinity_image,ScaleXToQuantum((size_t)
                      (i/map_info->red_mult),map_info->red_max),q);
                  SetPixelGreen(affinity_image,0,q);
                  if (map_info->green_max != 0)
                    SetPixelGreen(affinity_image,ScaleXToQuantum((size_t)
                      ((i/map_info->green_mult) % (map_info->green_max+1)),
                      map_info->green_max),q);
                  SetPixelBlue(affinity_image,0,q);
                  if (map_info->blue_max != 0)
                    SetPixelBlue(affinity_image,ScaleXToQuantum((size_t)
                      (i % map_info->green_mult),map_info->blue_max),q);
                  SetPixelAlpha(affinity_image,
                    TransparentAlpha,q);
                  q+=GetPixelChannels(affinity_image);
                }
                (void) SyncAuthenticPixels(affinity_image,exception);
                (void) RemapImage(resource_info->quantize_info,image,
                  affinity_image,exception);
              }
            XGetPixelInfo(display,visual_info,map_info,resource_info,image,
              pixel);
            (void) SetImageStorageClass(image,DirectClass,exception);
            affinity_image=DestroyImage(affinity_image);
          }
      if (IsEventLogging())
        {
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "Standard Colormap:");
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "  colormap id: 0x%lx",map_info->colormap);
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "  red, green, blue max: %lu %lu %lu",map_info->red_max,
            map_info->green_max,map_info->blue_max);
          (void) LogMagickEvent(X11Event,GetMagickModule(),
            "  red, green, blue mult: %lu %lu %lu",map_info->red_mult,
            map_info->green_mult,map_info->blue_mult);
        }
      return;
    }
  if ((visual_info->klass != DirectColor) &&
      (visual_info->klass != TrueColor))
    if ((image->storage_class == DirectClass) ||
        ((int) image->colors > visual_info->colormap_size))
      {
        QuantizeInfo
          quantize_info;

        /*
          Image has more colors than the visual supports.
        */
        quantize_info=(*resource_info->quantize_info);
        quantize_info.number_colors=(size_t) visual_info->colormap_size;
        (void) QuantizeImage(&quantize_info,image,exception);
      }
  /*
    Free previous and create new colormap.
  */
  (void) XFreeStandardColormap(display,visual_info,map_info,pixel);
  colormap=XDefaultColormap(display,visual_info->screen);
  if (visual_info->visual != XDefaultVisual(display,visual_info->screen))
    colormap=XCreateColormap(display,XRootWindow(display,visual_info->screen),
      visual_info->visual,visual_info->klass == DirectColor ?
      AllocAll : AllocNone);
  if (colormap == (Colormap) NULL)
    ThrowXWindowFatalException(ResourceLimitFatalError,"UnableToCreateColormap",
      image->filename);
  /*
    Initialize the map and pixel info structures.
  */
  XGetMapInfo(visual_info,colormap,map_info);
  XGetPixelInfo(display,visual_info,map_info,resource_info,image,pixel);
  /*
    Allocating colors in server colormap is based on visual class.
  */
  switch (visual_info->klass)
  {
    case StaticGray:
    case StaticColor:
    {
      /*
        Define Standard Colormap for StaticGray or StaticColor visual.
      */
      number_colors=image->colors;
      colors=(XColor *) AcquireQuantumMemory((size_t)
        visual_info->colormap_size,sizeof(*colors));
      if (colors == (XColor *) NULL)
        ThrowXWindowFatalException(ResourceLimitFatalError,
          "UnableToCreateColormap",image->filename);
      p=colors;
      color.flags=(char) (DoRed | DoGreen | DoBlue);
      for (i=0; i < (ssize_t) image->colors; i++)
      {
        color.red=ScaleQuantumToShort(XRedGamma(image->colormap[i].red));
        color.green=ScaleQuantumToShort(XGreenGamma(image->colormap[i].green));
        color.blue=ScaleQuantumToShort(XBlueGamma(image->colormap[i].blue));
        if (visual_info->klass != StaticColor)
          {
            gray_value=(unsigned short) XPixelIntensity(&color);
            color.red=gray_value;
            color.green=gray_value;
            color.blue=gray_value;
          }
        status=XAllocColor(display,colormap,&color);
        if (status == False)
          {
            colormap=XCopyColormapAndFree(display,colormap);
            (void) XAllocColor(display,colormap,&color);
          }
        pixel->pixels[i]=color.pixel;
        *p++=color;
      }
      break;
    }
    case GrayScale:
    case PseudoColor:
    {
      unsigned int
        colormap_type;

      /*
        Define Standard Colormap for GrayScale or PseudoColor visual.
      */
      number_colors=image->colors;
      colors=(XColor *) AcquireQuantumMemory((size_t)
        visual_info->colormap_size,sizeof(*colors));
      if (colors == (XColor *) NULL)
        ThrowXWindowFatalException(ResourceLimitFatalError,
          "UnableToCreateColormap",image->filename);
      /*
        Preallocate our GUI colors.
      */
      (void) XAllocColor(display,colormap,&pixel->foreground_color);
      (void) XAllocColor(display,colormap,&pixel->background_color);
      (void) XAllocColor(display,colormap,&pixel->border_color);
      (void) XAllocColor(display,colormap,&pixel->matte_color);
      (void) XAllocColor(display,colormap,&pixel->highlight_color);
      (void) XAllocColor(display,colormap,&pixel->shadow_color);
      (void) XAllocColor(display,colormap,&pixel->depth_color);
      (void) XAllocColor(display,colormap,&pixel->trough_color);
      for (i=0; i < MaxNumberPens; i++)
        (void) XAllocColor(display,colormap,&pixel->pen_colors[i]);
      /*
        Determine if image colors will "fit" into X server colormap.
      */
      colormap_type=resource_info->colormap;
      status=XAllocColorCells(display,colormap,MagickFalse,(unsigned long *)
        NULL,0,pixel->pixels,(unsigned int) image->colors);
      if (status != False)
        colormap_type=PrivateColormap;
      if (colormap_type == SharedColormap)
        {
          CacheView
            *image_view;

          DiversityPacket
            *diversity;

          int
            y;

          register int
            x;

          unsigned short
            index;

          XColor
            *server_colors;

          /*
            Define Standard colormap for shared GrayScale or PseudoColor visual.
          */
          diversity=(DiversityPacket *) AcquireQuantumMemory(image->colors,
            sizeof(*diversity));
          if (diversity == (DiversityPacket *) NULL)
            ThrowXWindowFatalException(ResourceLimitFatalError,
              "UnableToCreateColormap",image->filename);
          for (i=0; i < (ssize_t) image->colors; i++)
          {
            diversity[i].red=ClampToQuantum(image->colormap[i].red);
            diversity[i].green=ClampToQuantum(image->colormap[i].green);
            diversity[i].blue=ClampToQuantum(image->colormap[i].blue);
            diversity[i].index=(unsigned short) i;
            diversity[i].count=0;
          }
          image_view=AcquireAuthenticCacheView(image,exception);
          for (y=0; y < (int) image->rows; y++)
          {
            register int
              x;

            register const Quantum
              *magick_restrict p;

            p=GetCacheViewAuthenticPixels(image_view,0,(ssize_t) y,
              image->columns,1,exception);
            if (p == (const Quantum *) NULL)
              break;
            for (x=(int) image->columns-1; x >= 0; x--)
            {
              diversity[(ssize_t) GetPixelIndex(image,p)].count++;
              p+=GetPixelChannels(image);
            }
          }
          image_view=DestroyCacheView(image_view);
          /*
            Sort colors by decreasing intensity.
          */
          qsort((void *) diversity,image->colors,sizeof(*diversity),
            IntensityCompare);
          for (i=0; i < (ssize_t) image->colors; )
          {
            diversity[i].count<<=4;  /* increase this colors popularity */
            i+=MagickMax((int) (image->colors >> 4),2);
          }
          diversity[image->colors-1].count<<=4;
          qsort((void *) diversity,image->colors,sizeof(*diversity),
            PopularityCompare);
          /*
            Allocate colors.
          */
          p=colors;
          color.flags=(char) (DoRed | DoGreen | DoBlue);
          for (i=0; i < (ssize_t) image->colors; i++)
          {
            index=diversity[i].index;
            color.red=
              ScaleQuantumToShort(XRedGamma(image->colormap[index].red));
            color.green=
              ScaleQuantumToShort(XGreenGamma(image->colormap[index].green));
            color.blue=
              ScaleQuantumToShort(XBlueGamma(image->colormap[index].blue));
            if (visual_info->klass != PseudoColor)
              {
                gray_value=(unsigned short) XPixelIntensity(&color);
                color.red=gray_value;
                color.green=gray_value;
                color.blue=gray_value;
              }
            status=XAllocColor(display,colormap,&color);
            if (status == False)
              break;
            pixel->pixels[index]=color.pixel;
            *p++=color;
          }
          /*
            Read X server colormap.
          */
          server_colors=(XColor *) AcquireQuantumMemory((size_t)
            visual_info->colormap_size,sizeof(*server_colors));
          if (server_colors == (XColor *) NULL)
            ThrowXWindowFatalException(ResourceLimitFatalError,
              "UnableToCreateColormap",image->filename);
          for (x=visual_info->colormap_size-1; x >= 0; x--)
            server_colors[x].pixel=(size_t) x;
          (void) XQueryColors(display,colormap,server_colors,
            (int) MagickMin((unsigned int) visual_info->colormap_size,256));
          /*
            Select remaining colors from X server colormap.
          */
          for (; i < (ssize_t) image->colors; i++)
          {
            index=diversity[i].index;
            color.red=ScaleQuantumToShort(
              XRedGamma(image->colormap[index].red));
            color.green=ScaleQuantumToShort(
              XGreenGamma(image->colormap[index].green));
            color.blue=ScaleQuantumToShort(
              XBlueGamma(image->colormap[index].blue));
            if (visual_info->klass != PseudoColor)
              {
                gray_value=(unsigned short) XPixelIntensity(&color);
                color.red=gray_value;
                color.green=gray_value;
                color.blue=gray_value;
              }
            XBestPixel(display,colormap,server_colors,(unsigned int)
              visual_info->colormap_size,&color);
            pixel->pixels[index]=color.pixel;
            *p++=color;
          }
          if ((int) image->colors < visual_info->colormap_size)
            {
              /*
                Fill up colors array-- more choices for pen colors.
              */
              retain_colors=MagickMin((unsigned int)
               (visual_info->colormap_size-image->colors),256);
              for (i=0; i < (ssize_t) retain_colors; i++)
                *p++=server_colors[i];
              number_colors+=retain_colors;
            }
          server_colors=(XColor *) RelinquishMagickMemory(server_colors);
          diversity=(DiversityPacket *) RelinquishMagickMemory(diversity);
          break;
        }
      /*
        Define Standard colormap for private GrayScale or PseudoColor visual.
      */
      if (status == False)
        {
          /*
            Not enough colormap entries in the colormap-- Create a new colormap.
          */
          colormap=XCreateColormap(display,
            XRootWindow(display,visual_info->screen),visual_info->visual,
            AllocNone);
          if (colormap == (Colormap) NULL)
            ThrowXWindowFatalException(ResourceLimitFatalError,
              "UnableToCreateColormap",image->filename);
          map_info->colormap=colormap;
          if ((int) image->colors < visual_info->colormap_size)
            {
              /*
                Retain colors from the default colormap to help lessens the
                effects of colormap flashing.
              */
              retain_colors=MagickMin((unsigned int)
                (visual_info->colormap_size-image->colors),256);
              p=colors+image->colors;
              for (i=0; i < (ssize_t) retain_colors; i++)
              {
                p->pixel=(unsigned long) i;
                p++;
              }
              (void) XQueryColors(display,
                XDefaultColormap(display,visual_info->screen),
                colors+image->colors,(int) retain_colors);
              /*
                Transfer colors from default to private colormap.
              */
              (void) XAllocColorCells(display,colormap,MagickFalse,
                (unsigned long *) NULL,0,pixel->pixels,(unsigned int)
                retain_colors);
              p=colors+image->colors;
              for (i=0; i < (ssize_t) retain_colors; i++)
              {
                p->pixel=pixel->pixels[i];
                p++;
              }
              (void) XStoreColors(display,colormap,colors+image->colors,
                (int) retain_colors);
              number_colors+=retain_colors;
            }
          (void) XAllocColorCells(display,colormap,MagickFalse,
            (unsigned long *) NULL,0,pixel->pixels,(unsigned int)
            image->colors);
        }
      /*
        Store the image colormap.
      */
      p=colors;
      color.flags=(char) (DoRed | DoGreen | DoBlue);
      for (i=0; i < (ssize_t) image->colors; i++)
      {
        color.red=ScaleQuantumToShort(XRedGamma(image->colormap[i].red));
        color.green=ScaleQuantumToShort(XGreenGamma(image->colormap[i].green));
        color.blue=ScaleQuantumToShort(XBlueGamma(image->colormap[i].blue));
        if (visual_info->klass != PseudoColor)
          {
            gray_value=(unsigned short) XPixelIntensity(&color);
            color.red=gray_value;
            color.green=gray_value;
            color.blue=gray_value;
          }
        color.pixel=pixel->pixels[i];
        *p++=color;
      }
      (void) XStoreColors(display,colormap,colors,(int) image->colors);
      break;
    }
    case TrueColor:
    case DirectColor:
    default:
    {
      MagickBooleanType
        linear_colormap;

      /*
        Define Standard Colormap for TrueColor or DirectColor visual.
      */
      number_colors=(unsigned int) ((map_info->red_max*map_info->red_mult)+
        (map_info->green_max*map_info->green_mult)+
        (map_info->blue_max*map_info->blue_mult)+1);
      linear_colormap=(number_colors > 4096) ||
        (((int) (map_info->red_max+1) == visual_info->colormap_size) &&
         ((int) (map_info->green_max+1) == visual_info->colormap_size) &&
         ((int) (map_info->blue_max+1) == visual_info->colormap_size)) ?
         MagickTrue : MagickFalse;
      if (linear_colormap != MagickFalse)
        number_colors=(size_t) visual_info->colormap_size;
      /*
        Allocate color array.
      */
      colors=(XColor *) AcquireQuantumMemory(number_colors,sizeof(*colors));
      if (colors == (XColor *) NULL)
        ThrowXWindowFatalException(ResourceLimitFatalError,
          "UnableToCreateColormap",image->filename);
      /*
        Initialize linear color ramp.
      */
      p=colors;
      color.flags=(char) (DoRed | DoGreen | DoBlue);
      if (linear_colormap != MagickFalse)
        for (i=0; i < (ssize_t) number_colors; i++)
        {
          color.blue=(unsigned short) 0;
          if (map_info->blue_max != 0)
            color.blue=(unsigned short) ((size_t)
              ((65535L*(i % map_info->green_mult))/map_info->blue_max));
          color.green=color.blue;
          color.red=color.blue;
          color.pixel=XStandardPixel(map_info,&color);
          *p++=color;
        }
      else
        for (i=0; i < (ssize_t) number_colors; i++)
        {
          color.red=(unsigned short) 0;
          if (map_info->red_max != 0)
            color.red=(unsigned short) ((size_t)
              ((65535L*(i/map_info->red_mult))/map_info->red_max));
          color.green=(unsigned int) 0;
          if (map_info->green_max != 0)
            color.green=(unsigned short) ((size_t)
              ((65535L*((i/map_info->green_mult) % (map_info->green_max+1)))/
                map_info->green_max));
          color.blue=(unsigned short) 0;
          if (map_info->blue_max != 0)
            color.blue=(unsigned short) ((size_t)
              ((65535L*(i % map_info->green_mult))/map_info->blue_max));
          color.pixel=XStandardPixel(map_info,&color);
          *p++=color;
        }
      if ((visual_info->klass == DirectColor) &&
          (colormap != XDefaultColormap(display,visual_info->screen)))
        (void) XStoreColors(display,colormap,colors,(int) number_colors);
      else
        for (i=0; i < (ssize_t) number_colors; i++)
          (void) XAllocColor(display,colormap,&colors[i]);
      break;
    }
  }
  if ((visual_info->klass != DirectColor) &&
      (visual_info->klass != TrueColor))
    {
      /*
        Set foreground, background, border, etc. pixels.
      */
      XBestPixel(display,colormap,colors,(unsigned int) number_colors,
        &pixel->foreground_color);
      XBestPixel(display,colormap,colors,(unsigned int) number_colors,
        &pixel->background_color);
      if (pixel->background_color.pixel == pixel->foreground_color.pixel)
        {
          /*
            Foreground and background colors must differ.
          */
          pixel->background_color.red=(~pixel->foreground_color.red);
          pixel->background_color.green=
            (~pixel->foreground_color.green);
          pixel->background_color.blue=
            (~pixel->foreground_color.blue);
          XBestPixel(display,colormap,colors,(unsigned int) number_colors,
            &pixel->background_color);
        }
      XBestPixel(display,colormap,colors,(unsigned int) number_colors,
        &pixel->border_color);
      XBestPixel(display,colormap,colors,(unsigned int) number_colors,
        &pixel->matte_color);
      XBestPixel(display,colormap,colors,(unsigned int) number_colors,
        &pixel->highlight_color);
      XBestPixel(display,colormap,colors,(unsigned int) number_colors,
        &pixel->shadow_color);
      XBestPixel(display,colormap,colors,(unsigned int) number_colors,
        &pixel->depth_color);
      XBestPixel(display,colormap,colors,(unsigned int) number_colors,
        &pixel->trough_color);
      for (i=0; i < MaxNumberPens; i++)
      {
        XBestPixel(display,colormap,colors,(unsigned int) number_colors,
          &pixel->pen_colors[i]);
        pixel->pixels[image->colors+i]=pixel->pen_colors[i].pixel;
      }
      pixel->colors=(ssize_t) (image->colors+MaxNumberPens);
    }
  colors=(XColor *) RelinquishMagickMemory(colors);
  if (IsEventLogging())
    {
      (void) LogMagickEvent(X11Event,GetMagickModule(),"Standard Colormap:");
      (void) LogMagickEvent(X11Event,GetMagickModule(),"  colormap id: 0x%lx",
        map_info->colormap);
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "  red, green, blue max: %lu %lu %lu",map_info->red_max,
        map_info->green_max,map_info->blue_max);
      (void) LogMagickEvent(X11Event,GetMagickModule(),
        "  red, green, blue mult: %lu %lu %lu",map_info->red_mult,
        map_info->green_mult,map_info->blue_mult);
    }
}