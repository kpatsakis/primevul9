MagickExport MagickBooleanType OrderedDitherImage(Image *image,
  const char *threshold_map,ExceptionInfo *exception)
{
#define DitherImageTag  "Dither/Image"

  CacheView
    *image_view;

  char
    token[MagickPathExtent];

  const char
    *p;

  double
    levels[CompositePixelChannel];

  MagickBooleanType
    status;

  MagickOffsetType
    progress;

  register ssize_t
    i;

  ssize_t
    y;

  ThresholdMap
    *map;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  if (threshold_map == (const char *) NULL)
    return(MagickTrue);
  p=(char *) threshold_map;
  while (((isspace((int) ((unsigned char) *p)) != 0) || (*p == ',')) &&
         (*p != '\0'))
    p++;
  threshold_map=p;
  while (((isspace((int) ((unsigned char) *p)) == 0) && (*p != ',')) &&
         (*p != '\0'))
  {
    if ((p-threshold_map) >= (MagickPathExtent-1))
      break;
    token[p-threshold_map]=(*p);
    p++;
  }
  token[p-threshold_map]='\0';
  map=GetThresholdMap(token,exception);
  if (map == (ThresholdMap *) NULL)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),OptionError,
        "InvalidArgument","%s : '%s'","ordered-dither",threshold_map);
      return(MagickFalse);
    }
  for (i=0; i < MaxPixelChannels; i++)
    levels[i]=2.0;
  p=strchr((char *) threshold_map,',');
  if ((p != (char *) NULL) && (isdigit((int) ((unsigned char) *(++p))) != 0))
    {
      GetNextToken(p,&p,MagickPathExtent,token);
      for (i=0; (i < MaxPixelChannels); i++)
        levels[i]=StringToDouble(token,(char **) NULL);
      for (i=0; (*p != '\0') && (i < MaxPixelChannels); i++)
      {
        GetNextToken(p,&p,MagickPathExtent,token);
        if (*token == ',')
          GetNextToken(p,&p,MagickPathExtent,token);
        levels[i]=StringToDouble(token,(char **) NULL);
      }
    }
  for (i=0; i < MaxPixelChannels; i++)
    if (fabs(levels[i]) >= 1)
      levels[i]-=1.0;
  if (SetImageStorageClass(image,DirectClass,exception) == MagickFalse)
    return(MagickFalse);
  status=MagickTrue;
  progress=0;
  image_view=AcquireAuthenticCacheView(image,exception);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static) shared(progress,status) \
    magick_number_threads(image,image,image->rows,1)
#endif
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    register ssize_t
      x;

    register Quantum
      *magick_restrict q;

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
      register ssize_t
        i;

      ssize_t
        n;

      n=0;
      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        ssize_t
          level,
          threshold;

        PixelChannel channel = GetPixelChannelChannel(image,i);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        if ((traits & UpdatePixelTrait) == 0)
          continue;
        if (fabs(levels[n]) < MagickEpsilon)
          {
            n++;
            continue;
          }
        threshold=(ssize_t) (QuantumScale*q[i]*(levels[n]*(map->divisor-1)+1));
        level=threshold/(map->divisor-1);
        threshold-=level*(map->divisor-1);
        q[i]=ClampToQuantum((double) (level+(threshold >=
          map->levels[(x % map->width)+map->width*(y % map->height)]))*
          QuantumRange/levels[n]);
        n++;
      }
      q+=GetPixelChannels(image);
    }
    if (SyncCacheViewAuthenticPixels(image_view,exception) == MagickFalse)
      status=MagickFalse;
    if (image->progress_monitor != (MagickProgressMonitor) NULL)
      {
        MagickBooleanType
          proceed;

#if defined(MAGICKCORE_OPENMP_SUPPORT)
        #pragma omp atomic
#endif
        progress++;
        proceed=SetImageProgress(image,DitherImageTag,progress,image->rows);
        if (proceed == MagickFalse)
          status=MagickFalse;
      }
  }
  image_view=DestroyCacheView(image_view);
  map=DestroyThresholdMap(map);
  return(MagickTrue);
}