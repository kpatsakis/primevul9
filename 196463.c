static MagickBooleanType ClonePixelCacheRepository(
  CacheInfo *magick_restrict clone_info,CacheInfo *magick_restrict cache_info,
  ExceptionInfo *exception)
{
#define MaxCacheThreads  ((size_t) GetMagickResourceLimit(ThreadResource))
#define cache_number_threads(source,destination,chunk,multithreaded) \
  num_threads((multithreaded) == 0 ? 1 : \
    (((source)->type != MemoryCache) && \
     ((source)->type != MapCache)) || \
    (((destination)->type != MemoryCache) && \
     ((destination)->type != MapCache)) ? \
    MagickMax(MagickMin(GetMagickResourceLimit(ThreadResource),2),1) : \
    MagickMax(MagickMin((ssize_t) GetMagickResourceLimit(ThreadResource),(ssize_t) (chunk)/256),1))

  MagickBooleanType
    optimize,
    status;

  NexusInfo
    **magick_restrict cache_nexus,
    **magick_restrict clone_nexus;

  size_t
    length;

  ssize_t
    y;

  assert(cache_info != (CacheInfo *) NULL);
  assert(clone_info != (CacheInfo *) NULL);
  assert(exception != (ExceptionInfo *) NULL);
  if (cache_info->type == PingCache)
    return(MagickTrue);
  length=cache_info->number_channels*sizeof(*cache_info->channel_map);
  if ((cache_info->columns == clone_info->columns) &&
      (cache_info->rows == clone_info->rows) &&
      (cache_info->number_channels == clone_info->number_channels) &&
      (memcmp(cache_info->channel_map,clone_info->channel_map,length) == 0) &&
      (cache_info->metacontent_extent == clone_info->metacontent_extent))
    {
      /*
        Identical pixel cache morphology.
      */
      if (((cache_info->type == MemoryCache) ||
           (cache_info->type == MapCache)) &&
          ((clone_info->type == MemoryCache) ||
           (clone_info->type == MapCache)))
        {
          (void) memcpy(clone_info->pixels,cache_info->pixels,
            cache_info->number_channels*cache_info->columns*cache_info->rows*
            sizeof(*cache_info->pixels));
          if ((cache_info->metacontent_extent != 0) &&
              (clone_info->metacontent_extent != 0))
            (void) memcpy(clone_info->metacontent,cache_info->metacontent,
              cache_info->columns*cache_info->rows*
              clone_info->metacontent_extent*sizeof(unsigned char));
          return(MagickTrue);
        }
      if ((cache_info->type == DiskCache) && (clone_info->type == DiskCache))
        return(ClonePixelCacheOnDisk(cache_info,clone_info));
    }
  /*
    Mismatched pixel cache morphology.
  */
  cache_nexus=AcquirePixelCacheNexus(MaxCacheThreads);
  clone_nexus=AcquirePixelCacheNexus(MaxCacheThreads);
  if ((cache_nexus == (NexusInfo **) NULL) ||
      (clone_nexus == (NexusInfo **) NULL))
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  length=cache_info->number_channels*sizeof(*cache_info->channel_map);
  optimize=(cache_info->number_channels == clone_info->number_channels) &&
    (memcmp(cache_info->channel_map,clone_info->channel_map,length) == 0) ?
    MagickTrue : MagickFalse;
  length=(size_t) MagickMin(cache_info->number_channels*cache_info->columns,
    clone_info->number_channels*clone_info->columns);
  status=MagickTrue;
#if defined(MAGICKCORE_OPENMP_SUPPORT)
  #pragma omp parallel for schedule(static,4) shared(status) \
    cache_number_threads(cache_info,clone_info,cache_info->rows,1)
#endif
  for (y=0; y < (ssize_t) cache_info->rows; y++)
  {
    const int
      id = GetOpenMPThreadId();

    Quantum
      *pixels;

    RectangleInfo
      region;

    register ssize_t
      x;

    if (status == MagickFalse)
      continue;
    if (y >= (ssize_t) clone_info->rows)
      continue;
    region.width=cache_info->columns;
    region.height=1;
    region.x=0;
    region.y=y;
    pixels=SetPixelCacheNexusPixels(cache_info,ReadMode,&region,
      cache_nexus[id],exception);
    if (pixels == (Quantum *) NULL)
      continue;
    status=ReadPixelCachePixels(cache_info,cache_nexus[id],exception);
    if (status == MagickFalse)
      continue;
    region.width=clone_info->columns;
    pixels=SetPixelCacheNexusPixels(clone_info,WriteMode,&region,
      clone_nexus[id],exception);
    if (pixels == (Quantum *) NULL)
      continue;
    (void) ResetMagickMemory(clone_nexus[id]->pixels,0,(size_t)
      clone_nexus[id]->length);
    if (optimize != MagickFalse)
      (void) memcpy(clone_nexus[id]->pixels,cache_nexus[id]->pixels,length*
        sizeof(Quantum));
    else
      {
        register const Quantum
          *magick_restrict p;

        register Quantum
          *magick_restrict q;

        /*
          Mismatched pixel channel map.
        */
        p=cache_nexus[id]->pixels;
        q=clone_nexus[id]->pixels;
        for (x=0; x < (ssize_t) cache_info->columns; x++)
        {
          register ssize_t
            i;

          if (x == (ssize_t) clone_info->columns)
            break;
          for (i=0; i < (ssize_t) clone_info->number_channels; i++)
          {
            PixelChannel
              channel;

            PixelTrait
              traits;

            channel=clone_info->channel_map[i].channel;
            traits=cache_info->channel_map[channel].traits;
            if (traits != UndefinedPixelTrait)
              *q=*(p+cache_info->channel_map[channel].offset);
            q++;
          }
          p+=cache_info->number_channels;
        }
      }
    status=WritePixelCachePixels(clone_info,clone_nexus[id],exception);
  }
  if ((cache_info->metacontent_extent != 0) &&
      (clone_info->metacontent_extent != 0))
    {
      /*
        Clone metacontent.
      */
      length=(size_t) MagickMin(cache_info->metacontent_extent,
        clone_info->metacontent_extent);
#if defined(MAGICKCORE_OPENMP_SUPPORT)
      #pragma omp parallel for schedule(static,4) shared(status) \
        cache_number_threads(cache_info,clone_info,cache_info->rows,1)
#endif
      for (y=0; y < (ssize_t) cache_info->rows; y++)
      {
        const int
          id = GetOpenMPThreadId();

        Quantum
          *pixels;

        RectangleInfo
          region;

        if (status == MagickFalse)
          continue;
        if (y >= (ssize_t) clone_info->rows)
          continue;
        region.width=cache_info->columns;
        region.height=1;
        region.x=0;
        region.y=y;
        pixels=SetPixelCacheNexusPixels(cache_info,ReadMode,&region,
          cache_nexus[id],exception);
        if (pixels == (Quantum *) NULL)
          continue;
        status=ReadPixelCacheMetacontent(cache_info,cache_nexus[id],exception);
        if (status == MagickFalse)
          continue;
        region.width=clone_info->columns;
        pixels=SetPixelCacheNexusPixels(clone_info,WriteMode,&region,
          clone_nexus[id],exception);
        if (pixels == (Quantum *) NULL)
          continue;
        if ((clone_nexus[id]->metacontent != (void *) NULL) &&
            (cache_nexus[id]->metacontent != (void *) NULL))
          (void) memcpy(clone_nexus[id]->metacontent,
            cache_nexus[id]->metacontent,length*sizeof(unsigned char));
        status=WritePixelCacheMetacontent(clone_info,clone_nexus[id],exception);
      }
    }
  cache_nexus=DestroyPixelCacheNexus(cache_nexus,MaxCacheThreads);
  clone_nexus=DestroyPixelCacheNexus(clone_nexus,MaxCacheThreads);
  if (cache_info->debug != MagickFalse)
    {
      char
        message[MagickPathExtent];

      (void) FormatLocaleString(message,MagickPathExtent,"%s => %s",
        CommandOptionToMnemonic(MagickCacheOptions,(ssize_t) cache_info->type),
        CommandOptionToMnemonic(MagickCacheOptions,(ssize_t) clone_info->type));
      (void) LogMagickEvent(CacheEvent,GetMagickModule(),"%s",message);
    }
  return(status);
}