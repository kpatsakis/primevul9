static size_t WriteStreamImage(const Image *image,const void *pixels,
  const size_t columns)
{
  CacheInfo
    *cache_info;

  RectangleInfo
    extract_info;

  size_t
    length,
    packet_size;

  ssize_t
    count;

  StreamInfo
    *stream_info;

  (void) pixels;
  stream_info=(StreamInfo *) image->client_data;
  switch (stream_info->storage_type)
  {
    default: packet_size=sizeof(unsigned char); break;
    case CharPixel: packet_size=sizeof(unsigned char); break;
    case DoublePixel: packet_size=sizeof(double); break;
    case FloatPixel: packet_size=sizeof(float); break;
    case LongPixel: packet_size=sizeof(unsigned int); break;
    case LongLongPixel: packet_size=sizeof(MagickSizeType); break;
    case QuantumPixel: packet_size=sizeof(Quantum); break;
    case ShortPixel: packet_size=sizeof(unsigned short); break;
  }
  cache_info=(CacheInfo *) image->cache;
  assert(cache_info->signature == MagickCoreSignature);
  packet_size*=strlen(stream_info->map);
  length=packet_size*cache_info->columns*cache_info->rows;
  if (image != stream_info->image)
    {
      ImageInfo
        *write_info;

      /*
        Prepare stream for writing.
      */
      (void) RelinquishAlignedMemory(stream_info->pixels);
      stream_info->pixels=(unsigned char *) AcquireAlignedMemory(1,length);
      if (stream_info->pixels == (unsigned char *) NULL)
        return(0);
      (void) ResetMagickMemory(stream_info->pixels,0,length);
      stream_info->image=image;
      write_info=CloneImageInfo(stream_info->image_info);
      (void) SetImageInfo(write_info,1,stream_info->exception);
      if (write_info->extract != (char *) NULL)
        (void) ParseAbsoluteGeometry(write_info->extract,
          &stream_info->extract_info);
      stream_info->y=0;
      write_info=DestroyImageInfo(write_info);
    }
  extract_info=stream_info->extract_info;
  if ((extract_info.width == 0) || (extract_info.height == 0))
    {
      /*
        Write all pixels to stream.
      */
      (void) StreamImagePixels(stream_info,image,stream_info->exception);
      count=WriteBlob(stream_info->stream,length,stream_info->pixels);
      stream_info->y++;
      return(count == 0 ? 0 : columns);
    }
  if ((stream_info->y < extract_info.y) ||
      (stream_info->y >= (ssize_t) (extract_info.y+extract_info.height)))
    {
      stream_info->y++;
      return(columns);
    }
  /*
    Write a portion of the pixel row to the stream.
  */
  (void) StreamImagePixels(stream_info,image,stream_info->exception);
  length=packet_size*extract_info.width;
  count=WriteBlob(stream_info->stream,length,stream_info->pixels+packet_size*
    extract_info.x);
  stream_info->y++;
  return(count == 0 ? 0 : columns);
}