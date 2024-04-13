MagickPrivate void XGetPixelInfo(Display *display,
  const XVisualInfo *visual_info,const XStandardColormap *map_info,
  const XResourceInfo *resource_info,Image *image,XPixelInfo *pixel)
{
  static const char
    *PenColors[MaxNumberPens]=
    {
      "#000000000000",  /* black */
      "#00000000ffff",  /* blue */
      "#0000ffffffff",  /* cyan */
      "#0000ffff0000",  /* green */
      "#bdbdbdbdbdbd",  /* gray */
      "#ffff00000000",  /* red */
      "#ffff0000ffff",  /* magenta */
      "#ffffffff0000",  /* yellow */
      "#ffffffffffff",  /* white */
      "#bdbdbdbdbdbd",  /* gray */
      "#bdbdbdbdbdbd"   /* gray */
    };

  Colormap
    colormap;

  extern const char
    BorderColor[],
    ForegroundColor[];

  register ssize_t
    i;

  Status
    status;

  unsigned int
    packets;

  /*
    Initialize pixel info.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(display != (Display *) NULL);
  assert(visual_info != (XVisualInfo *) NULL);
  assert(map_info != (XStandardColormap *) NULL);
  assert(resource_info != (XResourceInfo *) NULL);
  assert(pixel != (XPixelInfo *) NULL);
  pixel->colors=0;
  if (image != (Image *) NULL)
    if (image->storage_class == PseudoClass)
      pixel->colors=(ssize_t) image->colors;
  packets=(unsigned int)
    MagickMax((int) pixel->colors,visual_info->colormap_size)+MaxNumberPens;
  if (pixel->pixels != (unsigned long *) NULL)
    pixel->pixels=(unsigned long *) RelinquishMagickMemory(pixel->pixels);
  pixel->pixels=(unsigned long *) AcquireQuantumMemory(packets,
    sizeof(*pixel->pixels));
  if (pixel->pixels == (unsigned long *) NULL)
    ThrowXWindowFatalException(ResourceLimitFatalError,"UnableToGetPixelInfo",
      image->filename);
  /*
    Set foreground color.
  */
  colormap=map_info->colormap;
  (void) XParseColor(display,colormap,(char *) ForegroundColor,
    &pixel->foreground_color);
  status=XParseColor(display,colormap,resource_info->foreground_color,
    &pixel->foreground_color);
  if (status == False)
    ThrowXWindowException(XServerError,"ColorIsNotKnownToServer",
      resource_info->foreground_color);
  pixel->foreground_color.pixel=
    XStandardPixel(map_info,&pixel->foreground_color);
  pixel->foreground_color.flags=(char) (DoRed | DoGreen | DoBlue);
  /*
    Set background color.
  */
  (void) XParseColor(display,colormap,"#d6d6d6d6d6d6",&pixel->background_color);
  status=XParseColor(display,colormap,resource_info->background_color,
    &pixel->background_color);
  if (status == False)
    ThrowXWindowException(XServerError,"ColorIsNotKnownToServer",
      resource_info->background_color);
  pixel->background_color.pixel=
    XStandardPixel(map_info,&pixel->background_color);
  pixel->background_color.flags=(char) (DoRed | DoGreen | DoBlue);
  /*
    Set border color.
  */
  (void) XParseColor(display,colormap,(char *) BorderColor,
    &pixel->border_color);
  status=XParseColor(display,colormap,resource_info->border_color,
    &pixel->border_color);
  if (status == False)
    ThrowXWindowException(XServerError,"ColorIsNotKnownToServer",
      resource_info->border_color);
  pixel->border_color.pixel=XStandardPixel(map_info,&pixel->border_color);
  pixel->border_color.flags=(char) (DoRed | DoGreen | DoBlue);
  /*
    Set matte color.
  */
  pixel->matte_color=pixel->background_color;
  if (resource_info->matte_color != (char *) NULL)
    {
      /*
        Matte color is specified as a X resource or command line argument.
      */
      status=XParseColor(display,colormap,resource_info->matte_color,
        &pixel->matte_color);
      if (status == False)
        ThrowXWindowException(XServerError,"ColorIsNotKnownToServer",
          resource_info->matte_color);
      pixel->matte_color.pixel=XStandardPixel(map_info,&pixel->matte_color);
      pixel->matte_color.flags=(char) (DoRed | DoGreen | DoBlue);
    }
  /*
    Set highlight color.
  */
  pixel->highlight_color.red=(unsigned short) (((double)
    pixel->matte_color.red*ScaleQuantumToShort(HighlightModulate))/65535L+
    (ScaleQuantumToShort((Quantum) (QuantumRange-HighlightModulate))));
  pixel->highlight_color.green=(unsigned short) (((double)
    pixel->matte_color.green*ScaleQuantumToShort(HighlightModulate))/65535L+
    (ScaleQuantumToShort((Quantum) (QuantumRange-HighlightModulate))));
  pixel->highlight_color.blue=(unsigned short) (((double)
    pixel->matte_color.blue*ScaleQuantumToShort(HighlightModulate))/65535L+
    (ScaleQuantumToShort((Quantum) (QuantumRange-HighlightModulate))));
  pixel->highlight_color.pixel=XStandardPixel(map_info,&pixel->highlight_color);
  pixel->highlight_color.flags=(char) (DoRed | DoGreen | DoBlue);
  /*
    Set shadow color.
  */
  pixel->shadow_color.red=(unsigned short) (((double)
    pixel->matte_color.red*ScaleQuantumToShort(ShadowModulate))/65535L);
  pixel->shadow_color.green=(unsigned short) (((double)
    pixel->matte_color.green*ScaleQuantumToShort(ShadowModulate))/65535L);
  pixel->shadow_color.blue=(unsigned short) (((double)
    pixel->matte_color.blue*ScaleQuantumToShort(ShadowModulate))/65535L);
  pixel->shadow_color.pixel=XStandardPixel(map_info,&pixel->shadow_color);
  pixel->shadow_color.flags=(char) (DoRed | DoGreen | DoBlue);
  /*
    Set depth color.
  */
  pixel->depth_color.red=(unsigned short) (((double)
    pixel->matte_color.red*ScaleQuantumToShort(DepthModulate))/65535L);
  pixel->depth_color.green=(unsigned short) (((double)
    pixel->matte_color.green*ScaleQuantumToShort(DepthModulate))/65535L);
  pixel->depth_color.blue=(unsigned short) (((double)
    pixel->matte_color.blue*ScaleQuantumToShort(DepthModulate))/65535L);
  pixel->depth_color.pixel=XStandardPixel(map_info,&pixel->depth_color);
  pixel->depth_color.flags=(char) (DoRed | DoGreen | DoBlue);
  /*
    Set trough color.
  */
  pixel->trough_color.red=(unsigned short) (((double)
    pixel->matte_color.red*ScaleQuantumToShort(TroughModulate))/65535L);
  pixel->trough_color.green=(unsigned short) (((double)
    pixel->matte_color.green*ScaleQuantumToShort(TroughModulate))/65535L);
  pixel->trough_color.blue=(unsigned short) (((double)
    pixel->matte_color.blue*ScaleQuantumToShort(TroughModulate))/65535L);
  pixel->trough_color.pixel=XStandardPixel(map_info,&pixel->trough_color);
  pixel->trough_color.flags=(char) (DoRed | DoGreen | DoBlue);
  /*
    Set pen color.
  */
  for (i=0; i < MaxNumberPens; i++)
  {
    (void) XParseColor(display,colormap,(char *) PenColors[i],
      &pixel->pen_colors[i]);
    status=XParseColor(display,colormap,resource_info->pen_colors[i],
      &pixel->pen_colors[i]);
    if (status == False)
      ThrowXWindowException(XServerError,"ColorIsNotKnownToServer",
        resource_info->pen_colors[i]);
    pixel->pen_colors[i].pixel=XStandardPixel(map_info,&pixel->pen_colors[i]);
    pixel->pen_colors[i].flags=(char) (DoRed | DoGreen | DoBlue);
  }
  pixel->box_color=pixel->background_color;
  pixel->pen_color=pixel->foreground_color;
  pixel->box_index=0;
  pixel->pen_index=1;
  if (image != (Image *) NULL)
    {
      if ((resource_info->gamma_correct != MagickFalse) &&
          (image->gamma != 0.0))
        {
          GeometryInfo
            geometry_info;

          MagickStatusType
            flags;

          /*
            Initialize map relative to display and image gamma.
          */
          flags=ParseGeometry(resource_info->display_gamma,&geometry_info);
          red_gamma=geometry_info.rho;
          green_gamma=geometry_info.sigma;
          if ((flags & SigmaValue) == 0)
            green_gamma=red_gamma;
          blue_gamma=geometry_info.xi;
          if ((flags & XiValue) == 0)
            blue_gamma=red_gamma;
          red_gamma*=image->gamma;
          green_gamma*=image->gamma;
          blue_gamma*=image->gamma;
        }
      if (image->storage_class == PseudoClass)
        {
          /*
            Initialize pixel array for images of type PseudoClass.
          */
          for (i=0; i < (ssize_t) image->colors; i++)
            pixel->pixels[i]=XGammaPacket(map_info,image->colormap+i);
          for (i=0; i < MaxNumberPens; i++)
            pixel->pixels[image->colors+i]=pixel->pen_colors[i].pixel;
          pixel->colors+=MaxNumberPens;
        }
    }
}