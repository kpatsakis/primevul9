MagickPrivate MagickBooleanType XRenderImage(Image *image,
  const DrawInfo *draw_info,const PointInfo *offset,TypeMetric *metrics)
{
  const char
    *client_name;

  DrawInfo
    cache_info;

  Display
    *display = (Display *) NULL;

  ImageInfo
    *image_info;

  MagickBooleanType
    status;

  size_t
    height,
    width;

  XAnnotateInfo
    annotate_info;

  XFontStruct
    *font_info;

  XPixelInfo
    pixel;

  XResourceInfo
    resource_info;

  XrmDatabase
    resource_database;

  XStandardColormap
    *map_info;

  XVisualInfo
    *visual_info;

  /*
    Open X server connection.
  */
  display=XOpenDisplay(draw_info->server_name);
  if (display == (Display *) NULL)
    {
      ThrowXWindowException(XServerError,"UnableToOpenXServer",
        draw_info->server_name);
      return(MagickFalse);
    }
  /*
    Get user defaults from X resource database.
  */
  (void) XSetErrorHandler(XError);
  image_info=AcquireImageInfo();
  client_name=GetClientName();
  resource_database=XGetResourceDatabase(display,client_name);
  XGetResourceInfo(image_info,resource_database,client_name,&resource_info);
  resource_info.close_server=MagickFalse;
  resource_info.colormap=PrivateColormap;
  resource_info.font=AcquireString(draw_info->font);
  resource_info.background_color=AcquireString("#ffffffffffff");
  resource_info.foreground_color=AcquireString("#000000000000");
  map_info=XAllocStandardColormap();
  visual_info=(XVisualInfo *) NULL;
  font_info=(XFontStruct *) NULL;
  pixel.pixels=(unsigned long *) NULL;
  if (map_info == (XStandardColormap *) NULL)
    {
      ThrowXWindowException(ResourceLimitError,"MemoryAllocationFailed",
        image->filename);
      return(MagickFalse);
    }
  /*
    Initialize visual info.
  */
  visual_info=XBestVisualInfo(display,map_info,&resource_info);
  if (visual_info == (XVisualInfo *) NULL)
    {
      XFreeResources(display,visual_info,map_info,&pixel,font_info,
        &resource_info,(XWindowInfo *) NULL);
      ThrowXWindowException(XServerError,"UnableToGetVisual",image->filename);
      return(MagickFalse);
    }
  map_info->colormap=(Colormap) NULL;
  /*
    Initialize Standard Colormap info.
  */
  XGetMapInfo(visual_info,XDefaultColormap(display,visual_info->screen),
    map_info);
  XGetPixelPacket(display,visual_info,map_info,&resource_info,(Image *) NULL,
    &pixel);
  pixel.annotate_context=XDefaultGC(display,visual_info->screen);
  /*
    Initialize font info.
  */
  font_info=XBestFont(display,&resource_info,MagickFalse);
  if (font_info == (XFontStruct *) NULL)
    {
      XFreeResources(display,visual_info,map_info,&pixel,font_info,
        &resource_info,(XWindowInfo *) NULL);
      ThrowXWindowException(XServerError,"UnableToLoadFont",draw_info->font);
      return(MagickFalse);
    }
  cache_info=(*draw_info);
  /*
    Initialize annotate info.
  */
  XGetAnnotateInfo(&annotate_info);
  annotate_info.stencil=ForegroundStencil;
  if (cache_info.font != draw_info->font)
    {
      /*
        Type name has changed.
      */
      (void) XFreeFont(display,font_info);
      (void) CloneString(&resource_info.font,draw_info->font);
      font_info=XBestFont(display,&resource_info,MagickFalse);
      if (font_info == (XFontStruct *) NULL)
        {
          ThrowXWindowException(XServerError,"UnableToLoadFont",
            draw_info->font);
          return(MagickFalse);
        }
    }
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(AnnotateEvent,GetMagickModule(),
      "Font %s; pointsize %g",draw_info->font != (char *) NULL ?
      draw_info->font : "none",draw_info->pointsize);
  cache_info=(*draw_info);
  annotate_info.font_info=font_info;
  annotate_info.text=(char *) draw_info->text;
  annotate_info.width=(unsigned int) XTextWidth(font_info,draw_info->text,(int)
    strlen(draw_info->text));
  annotate_info.height=(unsigned int) font_info->ascent+font_info->descent;
  metrics->pixels_per_em.x=(double) font_info->max_bounds.width;
  metrics->pixels_per_em.y=(double) font_info->ascent+font_info->descent;
  metrics->ascent=(double) font_info->ascent+4;
  metrics->descent=(double) (-font_info->descent);
  metrics->width=annotate_info.width/ExpandAffine(&draw_info->affine);
  metrics->height=font_info->ascent+font_info->descent;
  metrics->max_advance=(double) font_info->max_bounds.width;
  metrics->bounds.x1=0.0;
  metrics->bounds.y1=metrics->descent;
  metrics->bounds.x2=metrics->ascent+metrics->descent;
  metrics->bounds.y2=metrics->ascent+metrics->descent;
  metrics->underline_position=(-2.0);
  metrics->underline_thickness=1.0;
  if (draw_info->render == MagickFalse)
    return(MagickTrue);
  if (draw_info->fill.opacity == TransparentOpacity)
    return(MagickTrue);
  /*
    Render fill color.
  */
  width=annotate_info.width;
  height=annotate_info.height;
  if ((draw_info->affine.rx != 0.0) || (draw_info->affine.ry != 0.0))
    {
      if (((draw_info->affine.sx-draw_info->affine.sy) == 0.0) &&
          ((draw_info->affine.rx+draw_info->affine.ry) == 0.0))
        annotate_info.degrees=(180.0/MagickPI)*
          atan2(draw_info->affine.rx,draw_info->affine.sx);
    }
  (void) FormatLocaleString(annotate_info.geometry,MaxTextExtent,
    "%.20gx%.20g%+.20g%+.20g",(double) width,(double) height,
    ceil(offset->x-0.5),ceil(offset->y-metrics->ascent-metrics->descent+
    draw_info->interline_spacing-0.5));
  pixel.pen_color.red=ScaleQuantumToShort(draw_info->fill.red);
  pixel.pen_color.green=ScaleQuantumToShort(draw_info->fill.green);
  pixel.pen_color.blue=ScaleQuantumToShort(draw_info->fill.blue);
  status=XAnnotateImage(display,&pixel,&annotate_info,image);
  if (status == 0)
    {
      ThrowXWindowException(ResourceLimitError,"MemoryAllocationFailed",
        image->filename);
      return(MagickFalse);
    }
  return(MagickTrue);
}