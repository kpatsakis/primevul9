static MagickBooleanType WritePixelCachePixels(
  CacheInfo *magick_restrict cache_info,NexusInfo *magick_restrict nexus_info,
  ExceptionInfo *exception)
{
  MagickOffsetType
    count,
    offset;

  MagickSizeType
    extent,
    length;

  register const Quantum
    *magick_restrict p;

  register ssize_t
    y;

  size_t
    rows;

  if (nexus_info->authentic_pixel_cache != MagickFalse)
    return(MagickTrue);
  offset=(MagickOffsetType) nexus_info->region.y*cache_info->columns+
    nexus_info->region.x;
  length=(MagickSizeType) cache_info->number_channels*nexus_info->region.width*
    sizeof(Quantum);
  extent=length*nexus_info->region.height;
  rows=nexus_info->region.height;
  y=0;
  p=nexus_info->pixels;
  switch (cache_info->type)
  {
    case MemoryCache:
    case MapCache:
    {
      register Quantum
        *magick_restrict q;

      /*
        Write pixels to memory.
      */
      if ((cache_info->columns == nexus_info->region.width) &&
          (extent == (MagickSizeType) ((size_t) extent)))
        {
          length=extent;
          rows=1UL;
        }
      q=cache_info->pixels+offset*cache_info->number_channels;
      for (y=0; y < (ssize_t) rows; y++)
      {
        (void) memcpy(q,p,(size_t) length);
        p+=cache_info->number_channels*nexus_info->region.width;
        q+=cache_info->number_channels*cache_info->columns;
      }
      break;
    }
    case DiskCache:
    {
      /*
        Write pixels to disk.
      */
      LockSemaphoreInfo(cache_info->file_semaphore);
      if (OpenPixelCacheOnDisk(cache_info,IOMode) == MagickFalse)
        {
          ThrowFileException(exception,FileOpenError,"UnableToOpenFile",
            cache_info->cache_filename);
          UnlockSemaphoreInfo(cache_info->file_semaphore);
          return(MagickFalse);
        }
      if ((cache_info->columns == nexus_info->region.width) &&
          (extent <= MagickMaxBufferExtent))
        {
          length=extent;
          rows=1UL;
        }
      for (y=0; y < (ssize_t) rows; y++)
      {
        count=WritePixelCacheRegion(cache_info,cache_info->offset+offset*
          cache_info->number_channels*sizeof(*p),length,(const unsigned char *)
          p);
        if (count != (MagickOffsetType) length)
          break;
        p+=cache_info->number_channels*nexus_info->region.width;
        offset+=cache_info->columns;
      }
      if (IsFileDescriptorLimitExceeded() != MagickFalse)
        (void) ClosePixelCacheOnDisk(cache_info);
      UnlockSemaphoreInfo(cache_info->file_semaphore);
      break;
    }
    case DistributedCache:
    {
      RectangleInfo
        region;

      /*
        Write pixels to distributed cache.
      */
      LockSemaphoreInfo(cache_info->file_semaphore);
      region=nexus_info->region;
      if ((cache_info->columns != nexus_info->region.width) ||
          (extent > MagickMaxBufferExtent))
        region.height=1UL;
      else
        {
          length=extent;
          rows=1UL;
        }
      for (y=0; y < (ssize_t) rows; y++)
      {
        count=WriteDistributePixelCachePixels((DistributeCacheInfo *)
          cache_info->server_info,&region,length,(const unsigned char *) p);
        if (count != (MagickOffsetType) length)
          break;
        p+=cache_info->number_channels*nexus_info->region.width;
        region.y++;
      }
      UnlockSemaphoreInfo(cache_info->file_semaphore);
      break;
    }
    default:
      break;
  }
  if (y < (ssize_t) rows)
    {
      ThrowFileException(exception,CacheError,"UnableToWritePixelCache",
        cache_info->cache_filename);
      return(MagickFalse);
    }
  if ((cache_info->debug != MagickFalse) &&
      (CacheTick(nexus_info->region.y,cache_info->rows) != MagickFalse))
    (void) LogMagickEvent(CacheEvent,GetMagickModule(),
      "%s[%.20gx%.20g%+.20g%+.20g]",cache_info->filename,(double)
      nexus_info->region.width,(double) nexus_info->region.height,(double)
      nexus_info->region.x,(double) nexus_info->region.y);
  return(MagickTrue);
}