MagickExport MagickBooleanType PersistPixelCache(Image *image,
  const char *filename,const MagickBooleanType attach,MagickOffsetType *offset,
  ExceptionInfo *exception)
{
  CacheInfo
    *magick_restrict cache_info,
    *magick_restrict clone_info;

  MagickBooleanType
    status;

  ssize_t
    page_size;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(image->cache != (void *) NULL);
  assert(filename != (const char *) NULL);
  assert(offset != (MagickOffsetType *) NULL);
  page_size=GetMagickPageSize();
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
#if defined(MAGICKCORE_OPENCL_SUPPORT)
  CopyOpenCLBuffer(cache_info);
#endif
  if (attach != MagickFalse)
    {
      /*
        Attach existing persistent pixel cache.
      */
      if (image->debug != MagickFalse)
        (void) LogMagickEvent(CacheEvent,GetMagickModule(),
          "attach persistent cache");
      (void) CopyMagickString(cache_info->cache_filename,filename,
        MagickPathExtent);
      cache_info->type=DiskCache;
      cache_info->offset=(*offset);
      if (OpenPixelCache(image,ReadMode,exception) == MagickFalse)
        return(MagickFalse);
      *offset+=cache_info->length+page_size-(cache_info->length % page_size);
      return(SyncImagePixelCache(image,exception));
    }
  /*
    Clone persistent pixel cache.
  */
  status=AcquireMagickResource(DiskResource,cache_info->length);
  if (status == MagickFalse)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),CacheError,
        "CacheResourcesExhausted","`%s'",image->filename);
      return(MagickFalse);
    }
  clone_info=(CacheInfo *) ClonePixelCache(cache_info);
  clone_info->type=DiskCache;
  (void) CopyMagickString(clone_info->cache_filename,filename,MagickPathExtent);
  clone_info->file=(-1);
  clone_info->storage_class=cache_info->storage_class;
  clone_info->colorspace=cache_info->colorspace;
  clone_info->alpha_trait=cache_info->alpha_trait;
  clone_info->read_mask=cache_info->read_mask;
  clone_info->write_mask=cache_info->write_mask;
  clone_info->columns=cache_info->columns;
  clone_info->rows=cache_info->rows;
  clone_info->number_channels=cache_info->number_channels;
  clone_info->metacontent_extent=cache_info->metacontent_extent;
  clone_info->mode=PersistMode;
  clone_info->length=cache_info->length;
  (void) memcpy(clone_info->channel_map,cache_info->channel_map,
    MaxPixelChannels*sizeof(*cache_info->channel_map));
  clone_info->offset=(*offset);
  status=ClonePixelCacheRepository(clone_info,cache_info,exception);
  *offset+=cache_info->length+page_size-(cache_info->length % page_size);
  clone_info=(CacheInfo *) DestroyPixelCache(clone_info);
  return(status);
}