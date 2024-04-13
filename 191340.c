static MagickBooleanType RenderPostscript(Image *image,
  const DrawInfo *draw_info,const PointInfo *offset,TypeMetric *metrics)
{
  char
    filename[MaxTextExtent],
    geometry[MaxTextExtent],
    *text;

  FILE
    *file;

  Image
    *annotate_image;

  ImageInfo
    *annotate_info;

  int
    unique_file;

  MagickBooleanType
    identity;

  PointInfo
    extent,
    point,
    resolution;

  register ssize_t
    i;

  size_t
    length;

  ssize_t
    y;

  /*
    Render label with a Postscript font.
  */
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(AnnotateEvent,GetMagickModule(),
      "Font %s; pointsize %g",draw_info->font != (char *) NULL ?
      draw_info->font : "none",draw_info->pointsize);
  file=(FILE *) NULL;
  unique_file=AcquireUniqueFileResource(filename);
  if (unique_file != -1)
    file=fdopen(unique_file,"wb");
  if ((unique_file == -1) || (file == (FILE *) NULL))
    {
      ThrowFileException(&image->exception,FileOpenError,"UnableToOpenFile",
        filename);
      return(MagickFalse);
    }
  (void) FormatLocaleFile(file,"%%!PS-Adobe-3.0\n");
  (void) FormatLocaleFile(file,"/ReencodeType\n");
  (void) FormatLocaleFile(file,"{\n");
  (void) FormatLocaleFile(file,"  findfont dup length\n");
  (void) FormatLocaleFile(file,
    "  dict begin { 1 index /FID ne {def} {pop pop} ifelse } forall\n");
  (void) FormatLocaleFile(file,
    "  /Encoding ISOLatin1Encoding def currentdict end definefont pop\n");
  (void) FormatLocaleFile(file,"} bind def\n");
  /*
    Sample to compute bounding box.
  */
  identity=(fabs(draw_info->affine.sx-draw_info->affine.sy) < MagickEpsilon) &&
    (fabs(draw_info->affine.rx) < MagickEpsilon) &&
    (fabs(draw_info->affine.ry) < MagickEpsilon) ? MagickTrue : MagickFalse;
  extent.x=0.0;
  extent.y=0.0;
  length=strlen(draw_info->text);
  for (i=0; i <= (ssize_t) (length+2); i++)
  {
    point.x=fabs(draw_info->affine.sx*i*draw_info->pointsize+
      draw_info->affine.ry*2.0*draw_info->pointsize);
    point.y=fabs(draw_info->affine.rx*i*draw_info->pointsize+
      draw_info->affine.sy*2.0*draw_info->pointsize);
    if (point.x > extent.x)
      extent.x=point.x;
    if (point.y > extent.y)
      extent.y=point.y;
  }
  (void) FormatLocaleFile(file,"%g %g moveto\n",identity  != MagickFalse ? 0.0 :
    extent.x/2.0,extent.y/2.0);
  (void) FormatLocaleFile(file,"%g %g scale\n",draw_info->pointsize,
    draw_info->pointsize);
  if ((draw_info->font == (char *) NULL) || (*draw_info->font == '\0') ||
      (strchr(draw_info->font,'/') != (char *) NULL))
    (void) FormatLocaleFile(file,
      "/Times-Roman-ISO dup /Times-Roman ReencodeType findfont setfont\n");
  else
    (void) FormatLocaleFile(file,
      "/%s-ISO dup /%s ReencodeType findfont setfont\n",draw_info->font,
      draw_info->font);
  (void) FormatLocaleFile(file,"[%g %g %g %g 0 0] concat\n",
    draw_info->affine.sx,-draw_info->affine.rx,-draw_info->affine.ry,
    draw_info->affine.sy);
  text=EscapeParenthesis(draw_info->text);
  if (identity == MagickFalse)
    (void) FormatLocaleFile(file,"(%s) stringwidth pop -0.5 mul -0.5 rmoveto\n",
      text);
  (void) FormatLocaleFile(file,"(%s) show\n",text);
  text=DestroyString(text);
  (void) FormatLocaleFile(file,"showpage\n");
  (void) fclose(file);
  (void) FormatLocaleString(geometry,MaxTextExtent,"%.20gx%.20g+0+0!",
    floor(extent.x+0.5),floor(extent.y+0.5));
  annotate_info=AcquireImageInfo();
  (void) FormatLocaleString(annotate_info->filename,MaxTextExtent,"ps:%s",
    filename);
  (void) CloneString(&annotate_info->page,geometry);
  if (draw_info->density != (char *) NULL)
    (void) CloneString(&annotate_info->density,draw_info->density);
  annotate_info->antialias=draw_info->text_antialias;
  annotate_image=ReadImage(annotate_info,&image->exception);
  CatchException(&image->exception);
  annotate_info=DestroyImageInfo(annotate_info);
  (void) RelinquishUniqueFileResource(filename);
  if (annotate_image == (Image *) NULL)
    return(MagickFalse);
  resolution.x=DefaultResolution;
  resolution.y=DefaultResolution;
  if (draw_info->density != (char *) NULL)
    {
      GeometryInfo
        geometry_info;

      MagickStatusType
        flags;

      flags=ParseGeometry(draw_info->density,&geometry_info);
      resolution.x=geometry_info.rho;
      resolution.y=geometry_info.sigma;
      if ((flags & SigmaValue) == 0)
        resolution.y=resolution.x;
    }
  if (identity == MagickFalse)
    (void) TransformImage(&annotate_image,"0x0",(char *) NULL);
  else
    {
      RectangleInfo
        crop_info;

      crop_info=GetImageBoundingBox(annotate_image,&annotate_image->exception);
      crop_info.height=(size_t) ((resolution.y/DefaultResolution)*
        ExpandAffine(&draw_info->affine)*draw_info->pointsize+0.5);
      crop_info.y=(ssize_t) ceil((resolution.y/DefaultResolution)*extent.y/8.0-
        0.5);
      (void) FormatLocaleString(geometry,MaxTextExtent,
        "%.20gx%.20g%+.20g%+.20g",(double) crop_info.width,(double)
        crop_info.height,(double) crop_info.x,(double) crop_info.y);
      (void) TransformImage(&annotate_image,geometry,(char *) NULL);
    }
  metrics->pixels_per_em.x=(resolution.y/DefaultResolution)*
    ExpandAffine(&draw_info->affine)*draw_info->pointsize;
  metrics->pixels_per_em.y=metrics->pixels_per_em.x;
  metrics->ascent=metrics->pixels_per_em.x;
  metrics->descent=metrics->pixels_per_em.y/-5.0;
  metrics->width=(double) annotate_image->columns/
    ExpandAffine(&draw_info->affine);
  metrics->height=1.152*metrics->pixels_per_em.x;
  metrics->max_advance=metrics->pixels_per_em.x;
  metrics->bounds.x1=0.0;
  metrics->bounds.y1=metrics->descent;
  metrics->bounds.x2=metrics->ascent+metrics->descent;
  metrics->bounds.y2=metrics->ascent+metrics->descent;
  metrics->underline_position=(-2.0);
  metrics->underline_thickness=1.0;
  if (draw_info->render == MagickFalse)
    {
      annotate_image=DestroyImage(annotate_image);
      return(MagickTrue);
    }
  if (draw_info->fill.opacity != TransparentOpacity)
    {
      ExceptionInfo
        *exception;

      MagickBooleanType
        sync;

      PixelPacket
        fill_color;

      CacheView
        *annotate_view;

      /*
        Render fill color.
      */
      if (image->matte == MagickFalse)
        (void) SetImageAlphaChannel(image,OpaqueAlphaChannel);
      if (annotate_image->matte == MagickFalse)
        (void) SetImageAlphaChannel(annotate_image,OpaqueAlphaChannel);
      fill_color=draw_info->fill;
      exception=(&image->exception);
      annotate_view=AcquireAuthenticCacheView(annotate_image,exception);
      for (y=0; y < (ssize_t) annotate_image->rows; y++)
      {
        register ssize_t
          x;

        register PixelPacket
          *magick_restrict q;

        q=GetCacheViewAuthenticPixels(annotate_view,0,y,annotate_image->columns,
          1,exception);
        if (q == (PixelPacket *) NULL)
          break;
        for (x=0; x < (ssize_t) annotate_image->columns; x++)
        {
          (void) GetFillColor(draw_info,x,y,&fill_color);
          SetPixelAlpha(q,ClampToQuantum((((QuantumRange-GetPixelIntensity(
            annotate_image,q))*(QuantumRange-fill_color.opacity))/
            QuantumRange)));
          SetPixelRed(q,fill_color.red);
          SetPixelGreen(q,fill_color.green);
          SetPixelBlue(q,fill_color.blue);
          q++;
        }
        sync=SyncCacheViewAuthenticPixels(annotate_view,exception);
        if (sync == MagickFalse)
          break;
      }
      annotate_view=DestroyCacheView(annotate_view);
      (void) CompositeImage(image,OverCompositeOp,annotate_image,
        (ssize_t) ceil(offset->x-0.5),(ssize_t) ceil(offset->y-(metrics->ascent+
        metrics->descent)-0.5));
    }
  annotate_image=DestroyImage(annotate_image);
  return(MagickTrue);
}