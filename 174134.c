MagickExport MagickBooleanType ColorDecisionListImage(Image *image,
  const char *color_correction_collection,ExceptionInfo *exception)
{
#define ColorDecisionListCorrectImageTag  "ColorDecisionList/Image"

  typedef struct _Correction
  {
    double
      slope,
      offset,
      power;
  } Correction;

  typedef struct _ColorCorrection
  {
    Correction
      red,
      green,
      blue;

    double
      saturation;
  } ColorCorrection;

  CacheView
    *image_view;

  char
    token[MagickPathExtent];

  ColorCorrection
    color_correction;

  const char
    *content,
    *p;

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  PixelInfo
    *cdl_map;

  register ssize_t
    i;

  ssize_t
    y;

  XMLTreeInfo
    *cc,
    *ccc,
    *sat,
    *sop;

  /*
    Allocate and initialize cdl maps.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  if (color_correction_collection == (const char *) NULL)
    return(MagickFalse);
  ccc=NewXMLTree((const char *) color_correction_collection,exception);
  if (ccc == (XMLTreeInfo *) NULL)
    return(MagickFalse);
  cc=GetXMLTreeChild(ccc,"ColorCorrection");
  if (cc == (XMLTreeInfo *) NULL)
    {
      ccc=DestroyXMLTree(ccc);
      return(MagickFalse);
    }
  color_correction.red.slope=1.0;
  color_correction.red.offset=0.0;
  color_correction.red.power=1.0;
  color_correction.green.slope=1.0;
  color_correction.green.offset=0.0;
  color_correction.green.power=1.0;
  color_correction.blue.slope=1.0;
  color_correction.blue.offset=0.0;
  color_correction.blue.power=1.0;
  color_correction.saturation=0.0;
  sop=GetXMLTreeChild(cc,"SOPNode");
  if (sop != (XMLTreeInfo *) NULL)
    {
      XMLTreeInfo
        *offset,
        *power,
        *slope;

      slope=GetXMLTreeChild(sop,"Slope");
      if (slope != (XMLTreeInfo *) NULL)
        {
          content=GetXMLTreeContent(slope);
          p=(const char *) content;
          for (i=0; (*p != '\0') && (i < 3); i++)
          {
            GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              GetNextToken(p,&p,MagickPathExtent,token);
            switch (i)
            {
              case 0:
              {
                color_correction.red.slope=StringToDouble(token,(char **) NULL);
                break;
              }
              case 1:
              {
                color_correction.green.slope=StringToDouble(token,
                  (char **) NULL);
                break;
              }
              case 2:
              {
                color_correction.blue.slope=StringToDouble(token,
                  (char **) NULL);
                break;
              }
            }
          }
        }
      offset=GetXMLTreeChild(sop,"Offset");
      if (offset != (XMLTreeInfo *) NULL)
        {
          content=GetXMLTreeContent(offset);
          p=(const char *) content;
          for (i=0; (*p != '\0') && (i < 3); i++)
          {
            GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              GetNextToken(p,&p,MagickPathExtent,token);
            switch (i)
            {
              case 0:
              {
                color_correction.red.offset=StringToDouble(token,
                  (char **) NULL);
                break;
              }
              case 1:
              {
                color_correction.green.offset=StringToDouble(token,
                  (char **) NULL);
                break;
              }
              case 2:
              {
                color_correction.blue.offset=StringToDouble(token,
                  (char **) NULL);
                break;
              }
            }
          }
        }
      power=GetXMLTreeChild(sop,"Power");
      if (power != (XMLTreeInfo *) NULL)
        {
          content=GetXMLTreeContent(power);
          p=(const char *) content;
          for (i=0; (*p != '\0') && (i < 3); i++)
          {
            GetNextToken(p,&p,MagickPathExtent,token);
            if (*token == ',')
              GetNextToken(p,&p,MagickPathExtent,token);
            switch (i)
            {
              case 0:
              {
                color_correction.red.power=StringToDouble(token,(char **) NULL);
                break;
              }
              case 1:
              {
                color_correction.green.power=StringToDouble(token,
                  (char **) NULL);
                break;
              }
              case 2:
              {
                color_correction.blue.power=StringToDouble(token,
                  (char **) NULL);
                break;
              }
            }
          }
        }
    }
  sat=GetXMLTreeChild(cc,"SATNode");
  if (sat != (XMLTreeInfo *) NULL)
    {
      XMLTreeInfo
        *saturation;

      saturation=GetXMLTreeChild(sat,"Saturation");
      if (saturation != (XMLTreeInfo *) NULL)
        {
          content=GetXMLTreeContent(saturation);
          p=(const char *) content;
          GetNextToken(p,&p,MagickPathExtent,token);
          color_correction.saturation=StringToDouble(token,(char **) NULL);
        }
    }
  ccc=DestroyXMLTree(ccc);
  if (image->debug != MagickFalse)
    {
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
        "  Color Correction Collection:");
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
        "  color_correction.red.slope: %g",color_correction.red.slope);
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
        "  color_correction.red.offset: %g",color_correction.red.offset);
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
        "  color_correction.red.power: %g",color_correction.red.power);
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
        "  color_correction.green.slope: %g",color_correction.green.slope);
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
        "  color_correction.green.offset: %g",color_correction.green.offset);
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
        "  color_correction.green.power: %g",color_correction.green.power);
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
        "  color_correction.blue.slope: %g",color_correction.blue.slope);
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
        "  color_correction.blue.offset: %g",color_correction.blue.offset);
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
        "  color_correction.blue.power: %g",color_correction.blue.power);
      (void) LogMagickEvent(TransformEvent,GetMagickModule(),
        "  color_correction.saturation: %g",color_correction.saturation);
    }
  cdl_map=(PixelInfo *) AcquireQuantumMemory(MaxMap+1UL,sizeof(*cdl_map));
  if (cdl_map == (PixelInfo *) NULL)
    ThrowBinaryException(ResourceLimitError,"MemoryAllocationFailed",
      image->filename);
  for (i=0; i <= (ssize_t) MaxMap; i++)
  {
    cdl_map[i].red=(double) ScaleMapToQuantum((double)
      (MaxMap*(pow(color_correction.red.slope*i/MaxMap+
      color_correction.red.offset,color_correction.red.power))));
    cdl_map[i].green=(double) ScaleMapToQuantum((double)
      (MaxMap*(pow(color_correction.green.slope*i/MaxMap+
      color_correction.green.offset,color_correction.green.power))));
    cdl_map[i].blue=(double) ScaleMapToQuantum((double)
      (MaxMap*(pow(color_correction.blue.slope*i/MaxMap+
      color_correction.blue.offset,color_correction.blue.power))));
  }
  if (image->storage_class == PseudoClass)
    for (i=0; i < (ssize_t) image->colors; i++)
    {
      /*
        Apply transfer function to colormap.
      */
      double
        luma;

      luma=0.21267f*image->colormap[i].red+0.71526*image->colormap[i].green+
        0.07217f*image->colormap[i].blue;
      image->colormap[i].red=luma+color_correction.saturation*cdl_map[
        ScaleQuantumToMap(ClampToQuantum(image->colormap[i].red))].red-luma;
      image->colormap[i].green=luma+color_correction.saturation*cdl_map[
        ScaleQuantumToMap(ClampToQuantum(image->colormap[i].green))].green-luma;
      image->colormap[i].blue=luma+color_correction.saturation*cdl_map[
        ScaleQuantumToMap(ClampToQuantum(image->colormap[i].blue))].blue-luma;
    }
  /*
    Apply transfer function to image.
  */
  status=MagickTrue;
  progress=0;
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(progress,status) \
    magick_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    double
      luma;

    register Quantum
      *magick_restrict q;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    q=GetCacheViewAuthenticPixels(image_view,0,y,image->columns,1,exception);
    if (q == (Quantum *) NULL)
      {
        status=MagickFalse;
        continue;
      }
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      luma=0.21267f*GetPixelRed(image,q)+0.71526*GetPixelGreen(image,q)+
        0.07217f*GetPixelBlue(image,q);
      SetPixelRed(image,ClampToQuantum(luma+color_correction.saturation*
        (cdl_map[ScaleQuantumToMap(GetPixelRed(image,q))].red-luma)),q);
      SetPixelGreen(image,ClampToQuantum(luma+color_correction.saturation*
        (cdl_map[ScaleQuantumToMap(GetPixelGreen(image,q))].green-luma)),q);
      SetPixelBlue(image,ClampToQuantum(luma+color_correction.saturation*
        (cdl_map[ScaleQuantumToMap(GetPixelBlue(image,q))].blue-luma)),q);
      q+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp critical (MagickCore_ColorDecisionListImageChannel)
#endif
        proceed=SetImageProgress(image,ColorDecisionListCorrectImageTag,
          progress++,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  cdl_map=(PixelInfo *) RelinquishMagickMemory(cdl_map);
  return(status);
}