static MagickBooleanType WriteMIFFImage(const ImageInfo *image_info,
  Image *image,ExceptionInfo *exception)
{
#if defined(MAGICKCORE_BZLIB_DELEGATE)
  bz_stream
    bzip_info;
#endif

  char
    buffer[MagickPathExtent];

  CompressionType
    compression;

  const char
    *property,
    *value;

#if defined(MAGICKCORE_LZMA_DELEGATE)
  lzma_allocator
    allocator;

  lzma_stream
    initialize_lzma = LZMA_STREAM_INIT,
    lzma_info;
#endif

  MagickBooleanType
    status;

  MagickOffsetType
    scene;

  PixelInfo
    pixel,
    target;

  QuantumInfo
    *quantum_info;

  QuantumType
    quantum_type;

  register ssize_t
    i;

  size_t
    imageListLength,
    length,
    packet_size;

  ssize_t
    y;

  unsigned char
    *compress_pixels,
    *pixels,
    *q;

#if defined(MAGICKCORE_ZLIB_DELEGATE)
  z_stream
    zip_info;
#endif

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  scene=0;
  imageListLength=GetImageListLength(image);
  do
  {
    /*
      Allocate image pixels.
    */
    if ((image->storage_class == PseudoClass) &&
        (image->colors > (size_t) (GetQuantumRange(image->depth)+1)))
      (void) SetImageStorageClass(image,DirectClass,exception);
    image->depth=image->depth <= 8 ? 8UL : image->depth <= 16 ? 16UL :
      image->depth <= 32 ? 32UL : 64UL;
    quantum_info=AcquireQuantumInfo(image_info,image);
    if (quantum_info == (QuantumInfo *) NULL)
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    if ((image->storage_class != PseudoClass) && (image->depth >= 16) &&
        (quantum_info->format == UndefinedQuantumFormat) &&
        (IsHighDynamicRangeImage(image,exception) != MagickFalse))
      {
        status=SetQuantumFormat(image,quantum_info,FloatingPointQuantumFormat);
        if (status == MagickFalse)
          {
            quantum_info=DestroyQuantumInfo(quantum_info);
            ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
          }
      }
    else
      if (image->depth < 16)    
        (void) DeleteImageProperty(image,"quantum:format");
    compression=UndefinedCompression;
    if (image_info->compression != UndefinedCompression)
      compression=image_info->compression;
    switch (compression)
    {
#if !defined(MAGICKCORE_LZMA_DELEGATE)
      case LZMACompression: compression=NoCompression; break;
#endif
#if !defined(MAGICKCORE_ZLIB_DELEGATE)
      case LZWCompression:
      case ZipCompression: compression=NoCompression; break;
#endif
#if !defined(MAGICKCORE_BZLIB_DELEGATE)
      case BZipCompression: compression=NoCompression; break;
#endif
      case RLECompression:
      {
        if (quantum_info->format == FloatingPointQuantumFormat)
          compression=NoCompression;
        GetPixelInfo(image,&target);
        break;
      }
      default:
        break;
    }
    packet_size=(size_t) (quantum_info->depth/8);
    if (image->storage_class == DirectClass)
      packet_size=(size_t) (3*quantum_info->depth/8);
    if (IsGrayColorspace(image->colorspace) != MagickFalse)
      packet_size=(size_t) (quantum_info->depth/8);
    if (image->alpha_trait != UndefinedPixelTrait)
      packet_size+=quantum_info->depth/8;
    if (image->colorspace == CMYKColorspace)
      packet_size+=quantum_info->depth/8;
    if (compression == RLECompression)
      packet_size++;
    length=MagickMax(BZipMaxExtent(packet_size*image->columns),ZipMaxExtent(
      packet_size*image->columns));
    if ((compression == BZipCompression) || (compression == ZipCompression))
      if (length != (size_t) ((unsigned int) length))
        compression=NoCompression;
    compress_pixels=(unsigned char *) AcquireQuantumMemory(length,
      sizeof(*compress_pixels));
    if (compress_pixels == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    /*
      Write MIFF header.
    */
    (void) WriteBlobString(image,"id=ImageMagick  version=1.0\n");
    (void) FormatLocaleString(buffer,MagickPathExtent,
      "class=%s  colors=%.20g  alpha-trait=%s\n",CommandOptionToMnemonic(
      MagickClassOptions,image->storage_class),(double) image->colors,
      CommandOptionToMnemonic(MagickPixelTraitOptions,(ssize_t)
      image->alpha_trait));
    (void) WriteBlobString(image,buffer);
    if (image->alpha_trait != UndefinedPixelTrait)
      (void) WriteBlobString(image,"matte=True\n");
    (void) FormatLocaleString(buffer,MagickPathExtent,
      "columns=%.20g  rows=%.20g  depth=%.20g\n",(double) image->columns,
      (double) image->rows,(double) image->depth);
    (void) WriteBlobString(image,buffer);
    if (image->type != UndefinedType)
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,"type=%s\n",
          CommandOptionToMnemonic(MagickTypeOptions,image->type));
        (void) WriteBlobString(image,buffer);
      }
    if (image->colorspace != UndefinedColorspace)
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,"colorspace=%s\n",
          CommandOptionToMnemonic(MagickColorspaceOptions,image->colorspace));
        (void) WriteBlobString(image,buffer);
      }
    if (image->intensity != UndefinedPixelIntensityMethod)
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,
          "pixel-intensity=%s\n",CommandOptionToMnemonic(
          MagickPixelIntensityOptions,image->intensity));
        (void) WriteBlobString(image,buffer);
      }
    if (image->endian != UndefinedEndian)
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,"endian=%s\n",
          CommandOptionToMnemonic(MagickEndianOptions,image->endian));
        (void) WriteBlobString(image,buffer);
      }
    if (compression != UndefinedCompression)
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,"compression=%s  "
          "quality=%.20g\n",CommandOptionToMnemonic(MagickCompressOptions,
          compression),(double) image->quality);
        (void) WriteBlobString(image,buffer);
      }
    if (image->units != UndefinedResolution)
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,"units=%s\n",
          CommandOptionToMnemonic(MagickResolutionOptions,image->units));
        (void) WriteBlobString(image,buffer);
      }
    if ((image->resolution.x != 0) || (image->resolution.y != 0))
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,
          "resolution=%gx%g\n",image->resolution.x,image->resolution.y);
        (void) WriteBlobString(image,buffer);
      }
    if ((image->page.width != 0) || (image->page.height != 0))
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,
          "page=%.20gx%.20g%+.20g%+.20g\n",(double) image->page.width,(double)
          image->page.height,(double) image->page.x,(double) image->page.y);
        (void) WriteBlobString(image,buffer);
      }
    else
      if ((image->page.x != 0) || (image->page.y != 0))
        {
          (void) FormatLocaleString(buffer,MagickPathExtent,"page=%+ld%+ld\n",
            (long) image->page.x,(long) image->page.y);
          (void) WriteBlobString(image,buffer);
        }
    if ((image->tile_offset.x != 0) || (image->tile_offset.y != 0))
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,
          "tile-offset=%+ld%+ld\n",(long) image->tile_offset.x,(long)
          image->tile_offset.y);
        (void) WriteBlobString(image,buffer);
      }
    if ((GetNextImageInList(image) != (Image *) NULL) ||
        (GetPreviousImageInList(image) != (Image *) NULL))
      {
        if (image->scene == 0)
          (void) FormatLocaleString(buffer,MagickPathExtent,"iterations=%.20g  "
            "delay=%.20g  ticks-per-second=%.20g\n",(double) image->iterations,
            (double) image->delay,(double) image->ticks_per_second);
        else
          (void) FormatLocaleString(buffer,MagickPathExtent,"scene=%.20g  "
            "iterations=%.20g  delay=%.20g  ticks-per-second=%.20g\n",(double)
            image->scene,(double) image->iterations,(double) image->delay,
            (double) image->ticks_per_second);
        (void) WriteBlobString(image,buffer);
      }
    else
      {
        if (image->scene != 0)
          {
            (void) FormatLocaleString(buffer,MagickPathExtent,"scene=%.20g\n",
              (double) image->scene);
            (void) WriteBlobString(image,buffer);
          }
        if (image->iterations != 0)
          {
            (void) FormatLocaleString(buffer,MagickPathExtent,
              "iterations=%.20g\n",(double) image->iterations);
            (void) WriteBlobString(image,buffer);
          }
        if (image->delay != 0)
          {
            (void) FormatLocaleString(buffer,MagickPathExtent,"delay=%.20g\n",
              (double) image->delay);
            (void) WriteBlobString(image,buffer);
          }
        if (image->ticks_per_second != UndefinedTicksPerSecond)
          {
            (void) FormatLocaleString(buffer,MagickPathExtent,
              "ticks-per-second=%.20g\n",(double) image->ticks_per_second);
            (void) WriteBlobString(image,buffer);
          }
      }
    if (image->gravity != UndefinedGravity)
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,"gravity=%s\n",
          CommandOptionToMnemonic(MagickGravityOptions,image->gravity));
        (void) WriteBlobString(image,buffer);
      }
    if (image->dispose != UndefinedDispose)
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,"dispose=%s\n",
          CommandOptionToMnemonic(MagickDisposeOptions,image->dispose));
        (void) WriteBlobString(image,buffer);
      }
    if (image->rendering_intent != UndefinedIntent)
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,
          "rendering-intent=%s\n",CommandOptionToMnemonic(MagickIntentOptions,
          image->rendering_intent));
        (void) WriteBlobString(image,buffer);
      }
    if (image->gamma != 0.0)
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,"gamma=%g\n",
          image->gamma);
        (void) WriteBlobString(image,buffer);
      }
    if (image->chromaticity.white_point.x != 0.0)
      {
        /*
          Note chomaticity points.
        */
        (void) FormatLocaleString(buffer,MagickPathExtent,"red-primary=%g,"
          "%g  green-primary=%g,%g  blue-primary=%g,%g\n",
          image->chromaticity.red_primary.x,image->chromaticity.red_primary.y,
          image->chromaticity.green_primary.x,
          image->chromaticity.green_primary.y,
          image->chromaticity.blue_primary.x,
          image->chromaticity.blue_primary.y);
        (void) WriteBlobString(image,buffer);
        (void) FormatLocaleString(buffer,MagickPathExtent,
          "white-point=%g,%g\n",image->chromaticity.white_point.x,
          image->chromaticity.white_point.y);
        (void) WriteBlobString(image,buffer);
      }
    if (image->orientation != UndefinedOrientation)
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,"orientation=%s\n",
          CommandOptionToMnemonic(MagickOrientationOptions,image->orientation));
        (void) WriteBlobString(image,buffer);
      }
    if (image->profiles != (void *) NULL)
      {
        const char
          *name;

        const StringInfo
          *profile;

        /*
          Write image profile names.
        */
        ResetImageProfileIterator(image);
        for (name=GetNextImageProfile(image); name != (const char *) NULL; )
        {
          profile=GetImageProfile(image,name);
          if (profile != (StringInfo *) NULL)
            {
              (void) FormatLocaleString(buffer,MagickPathExtent,"profile=%s\n",
                name);
              (void) WriteBlobString(image,buffer);
            }
          name=GetNextImageProfile(image);
        }
      }
    if (image->montage != (char *) NULL)
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,"montage=%s\n",
          image->montage);
        (void) WriteBlobString(image,buffer);
      }
    if (quantum_info->format == FloatingPointQuantumFormat)
      (void) SetImageProperty(image,"quantum:format","floating-point",
        exception);
    ResetImagePropertyIterator(image);
    property=GetNextImageProperty(image);
    while (property != (const char *) NULL)
    {
      (void) FormatLocaleString(buffer,MagickPathExtent,"%s=",property);
      (void) WriteBlobString(image,buffer);
      value=GetImageProperty(image,property,exception);
      if (value != (const char *) NULL)
        {
          length=strlen(value);
          for (i=0; i < (ssize_t) length; i++)
            if ((isspace((int) ((unsigned char) value[i])) != 0) ||
                (value[i] == '}'))
              break;
          if ((i == (ssize_t) length) && (i != 0))
            (void) WriteBlob(image,length,(const unsigned char *) value);
          else
            {
              (void) WriteBlobByte(image,'{');
              if (strchr(value,'}') == (char *) NULL)
                (void) WriteBlob(image,length,(const unsigned char *) value);
              else
                for (i=0; i < (ssize_t) length; i++)
                {
                  if (value[i] == (int) '}')
                    (void) WriteBlobByte(image,'\\');
                  (void) WriteBlobByte(image,(unsigned char) value[i]);
                }
              (void) WriteBlobByte(image,'}');
            }
        }
      (void) WriteBlobByte(image,'\n');
      property=GetNextImageProperty(image);
    }
    (void) WriteBlobString(image,"\f\n:\032");
    if (image->montage != (char *) NULL)
      {
        /*
          Write montage tile directory.
        */
        if (image->directory != (char *) NULL)
          (void) WriteBlob(image,strlen(image->directory),(unsigned char *)
            image->directory);
        (void) WriteBlobByte(image,'\0');
      }
    if (image->profiles != 0)
      {
        const char
          *name;

        const StringInfo
          *profile;

        /*
          Write image profile blob.
        */
        ResetImageProfileIterator(image);
        name=GetNextImageProfile(image);
        while (name != (const char *) NULL)
        {
          profile=GetImageProfile(image,name);
          (void) WriteBlobMSBLong(image,(unsigned int)
            GetStringInfoLength(profile));
          (void) WriteBlob(image,GetStringInfoLength(profile),
            GetStringInfoDatum(profile));
          name=GetNextImageProfile(image);
        }
      }
    if (image->storage_class == PseudoClass)
      {
        size_t
          colormap_size;

        unsigned char
          *colormap;

        /*
          Allocate colormap.
        */
        colormap_size=(size_t) (3*quantum_info->depth/8);
        colormap=(unsigned char *) AcquireQuantumMemory(image->colors,
          colormap_size*sizeof(*colormap));
        if (colormap == (unsigned char *) NULL)
          ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
        /*
          Write colormap to file.
        */
        q=colormap;
        for (i=0; i < (ssize_t) image->colors; i++)
        {
          switch (quantum_info->depth)
          {
            default:
              ThrowWriterException(CorruptImageError,"ImageDepthNotSupported");
            case 32:
            {
              register unsigned int
                long_pixel;

              long_pixel=ScaleQuantumToLong((Quantum)
                image->colormap[i].red);
              q=PopLongPixel(MSBEndian,long_pixel,q);
              long_pixel=ScaleQuantumToLong((Quantum)
                image->colormap[i].green);
              q=PopLongPixel(MSBEndian,long_pixel,q);
              long_pixel=ScaleQuantumToLong((Quantum)
                image->colormap[i].blue);
              q=PopLongPixel(MSBEndian,long_pixel,q);
              break;
            }
            case 16:
            {
              register unsigned short
                short_pixel;

              short_pixel=ScaleQuantumToShort((Quantum)
                image->colormap[i].red);
              q=PopShortPixel(MSBEndian,short_pixel,q);
              short_pixel=ScaleQuantumToShort((Quantum)
                image->colormap[i].green);
              q=PopShortPixel(MSBEndian,short_pixel,q);
              short_pixel=ScaleQuantumToShort((Quantum)
                image->colormap[i].blue);
              q=PopShortPixel(MSBEndian,short_pixel,q);
              break;
            }
            case 8:
            {
              register unsigned char
                char_pixel;

              char_pixel=(unsigned char) ScaleQuantumToChar((Quantum)
                image->colormap[i].red);
              q=PopCharPixel(char_pixel,q);
              char_pixel=(unsigned char) ScaleQuantumToChar((Quantum)
                image->colormap[i].green);
              q=PopCharPixel(char_pixel,q);
              char_pixel=(unsigned char) ScaleQuantumToChar((Quantum)
                image->colormap[i].blue);
              q=PopCharPixel(char_pixel,q);
              break;
            }
          }
        }
        (void) WriteBlob(image,colormap_size*image->colors,colormap);
        colormap=(unsigned char *) RelinquishMagickMemory(colormap);
      }
    /*
      Write image pixels to file.
    */
    status=MagickTrue;
    switch (compression)
    {
#if defined(MAGICKCORE_BZLIB_DELEGATE)
      case BZipCompression:
      {
        int
          code;

        (void) memset(&bzip_info,0,sizeof(bzip_info));
        bzip_info.bzalloc=AcquireBZIPMemory;
        bzip_info.bzfree=RelinquishBZIPMemory;
        bzip_info.opaque=(void *) NULL;
        code=BZ2_bzCompressInit(&bzip_info,(int) (image->quality ==
          UndefinedCompressionQuality ? 7 : MagickMin(image->quality/10,9)),
          (int) image_info->verbose,0);
        if (code != BZ_OK)
          status=MagickFalse;
        break;
      }
#endif
#if defined(MAGICKCORE_LZMA_DELEGATE)
      case LZMACompression:
      {
        int
          code;

        allocator.alloc=AcquireLZMAMemory;
        allocator.free=RelinquishLZMAMemory;
        allocator.opaque=(void *) NULL;
        lzma_info=initialize_lzma;
        lzma_info.allocator=&allocator;
        code=lzma_easy_encoder(&lzma_info,(uint32_t) (image->quality/10),
          LZMA_CHECK_SHA256);
        if (code != LZMA_OK)
          status=MagickTrue;
        break;
      }
#endif
#if defined(MAGICKCORE_ZLIB_DELEGATE)
      case LZWCompression:
      case ZipCompression:
      {
        int
          code;

        zip_info.zalloc=AcquireZIPMemory;
        zip_info.zfree=RelinquishZIPMemory;
        zip_info.opaque=(void *) NULL;
        code=deflateInit(&zip_info,(int) (image->quality ==
          UndefinedCompressionQuality ? 7 : MagickMin(image->quality/10,9)));
        if (code != Z_OK)
          status=MagickFalse;
        break;
      }
#endif
      default:
        break;
    }
    quantum_type=GetQuantumType(image,exception);
    pixels=(unsigned char *) GetQuantumPixels(quantum_info);
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      register const Quantum
        *magick_restrict p;

      register ssize_t
        x;

      if (status == MagickFalse)
        break;
      p=GetVirtualPixels(image,0,y,image->columns,1,exception);
      if (p == (const Quantum *) NULL)
        break;
      q=pixels;
      switch (compression)
      {
#if defined(MAGICKCORE_BZLIB_DELEGATE)
        case BZipCompression:
        {
          bzip_info.next_in=(char *) pixels;
          bzip_info.avail_in=(unsigned int) (packet_size*image->columns);
          (void) ExportQuantumPixels(image,(CacheView *) NULL,quantum_info,
            quantum_type,pixels,exception);
          do
          {
            int
              code;

            bzip_info.next_out=(char *) compress_pixels;
            bzip_info.avail_out=(unsigned int) BZipMaxExtent(packet_size*
              image->columns);
            code=BZ2_bzCompress(&bzip_info,BZ_FLUSH);
            if (code != BZ_OK)
              status=MagickFalse;
            length=(size_t) (bzip_info.next_out-(char *) compress_pixels);
            if (length != 0)
              {
                (void) WriteBlobMSBLong(image,(unsigned int) length);
                (void) WriteBlob(image,length,compress_pixels);
              }
          } while (bzip_info.avail_in != 0);
          break;
        }
#endif
#if defined(MAGICKCORE_LZMA_DELEGATE)
        case LZMACompression:
        {
          lzma_info.next_in=pixels;
          lzma_info.avail_in=packet_size*image->columns;
          (void) ExportQuantumPixels(image,(CacheView *) NULL,quantum_info,
            quantum_type,pixels,exception);
          do
          {
            int
              code;

            lzma_info.next_out=compress_pixels;
            lzma_info.avail_out=packet_size*image->columns;
            code=lzma_code(&lzma_info,LZMA_RUN);
            if (code != LZMA_OK)
              status=MagickFalse;
            length=(size_t) (lzma_info.next_out-compress_pixels);
            if (length != 0)
              {
                (void) WriteBlobMSBLong(image,(unsigned int) length);
                (void) WriteBlob(image,length,compress_pixels);
              }
          } while (lzma_info.avail_in != 0);
          break;
        }
#endif
#if defined(MAGICKCORE_ZLIB_DELEGATE)
        case LZWCompression:
        case ZipCompression:
        {
          zip_info.next_in=pixels;
          zip_info.avail_in=(uInt) (packet_size*image->columns);
          (void) ExportQuantumPixels(image,(CacheView *) NULL,quantum_info,
            quantum_type,pixels,exception);
          do
          {
            int
              code;

            zip_info.next_out=compress_pixels;
            zip_info.avail_out=(uInt) ZipMaxExtent(packet_size*image->columns);
            code=deflate(&zip_info,Z_SYNC_FLUSH);
            if (code != Z_OK)
              status=MagickFalse;
            length=(size_t) (zip_info.next_out-compress_pixels);
            if (length != 0)
              {
                (void) WriteBlobMSBLong(image,(unsigned int) length);
                (void) WriteBlob(image,length,compress_pixels);
              }
          } while (zip_info.avail_in != 0);
          break;
        }
#endif
        case RLECompression:
        {
          length=0;
          GetPixelInfoPixel(image,p,&pixel);
          p+=GetPixelChannels(image);
          for (x=1; x < (ssize_t) image->columns; x++)
          {
            GetPixelInfoPixel(image,p,&target);
            if ((length < 255) &&
                (IsPixelInfoEquivalent(&pixel,&target) != MagickFalse))
              length++;
            else
              {
                q=PopRunlengthPacket(image,q,length,&pixel,exception);
                length=0;
              }
            GetPixelInfoPixel(image,p,&pixel);
            p+=GetPixelChannels(image);
          }
          q=PopRunlengthPacket(image,q,length,&pixel,exception);
          (void) WriteBlob(image,(size_t) (q-pixels),pixels);
          break;
        }
        default:
        {
          (void) ExportQuantumPixels(image,(CacheView *) NULL,quantum_info,
            quantum_type,pixels,exception);
          (void) WriteBlob(image,packet_size*image->columns,pixels);
          break;
        }
      }
      if (image->previous == (Image *) NULL)
        {
          status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
            image->rows);
          if (status == MagickFalse)
            break;
        }
    }
    switch (compression)
    {
#if defined(MAGICKCORE_BZLIB_DELEGATE)
      case BZipCompression:
      {
        int
          code;

        for ( ; ; )
        {
          if (status == MagickFalse)
            break;
          bzip_info.next_out=(char *) compress_pixels;
          bzip_info.avail_out=(unsigned int) BZipMaxExtent(packet_size*
            image->columns);
          code=BZ2_bzCompress(&bzip_info,BZ_FINISH);
          length=(size_t) (bzip_info.next_out-(char *) compress_pixels);
          if (length != 0)
            {
              (void) WriteBlobMSBLong(image,(unsigned int) length);
              (void) WriteBlob(image,length,compress_pixels);
            }
          if (code == BZ_STREAM_END)
            break;
        }
        code=BZ2_bzCompressEnd(&bzip_info);
        if (code != BZ_OK)
          status=MagickFalse;
        break;
      }
#endif
#if defined(MAGICKCORE_LZMA_DELEGATE)
      case LZMACompression:
      {
        int
          code;

        for ( ; ; )
        {
          if (status == MagickFalse)
            break;
          lzma_info.next_out=compress_pixels;
          lzma_info.avail_out=packet_size*image->columns;
          code=lzma_code(&lzma_info,LZMA_FINISH);
          length=(size_t) (lzma_info.next_out-compress_pixels);
          if (length > 6)
            {
              (void) WriteBlobMSBLong(image,(unsigned int) length);
              (void) WriteBlob(image,length,compress_pixels);
            }
          if (code == LZMA_STREAM_END)
            break;
        }
        lzma_end(&lzma_info);
        break;
      }
#endif
#if defined(MAGICKCORE_ZLIB_DELEGATE)
      case LZWCompression:
      case ZipCompression:
      {
        int
          code;

        for ( ; ; )
        {
          if (status == MagickFalse)
            break;
          zip_info.next_out=compress_pixels;
          zip_info.avail_out=(uInt) ZipMaxExtent(packet_size*image->columns);
          code=deflate(&zip_info,Z_FINISH);
          length=(size_t) (zip_info.next_out-compress_pixels);
          if (length > 6)
            {
              (void) WriteBlobMSBLong(image,(unsigned int) length);
              (void) WriteBlob(image,length,compress_pixels);
            }
          if (code == Z_STREAM_END)
            break;
        }
        code=deflateEnd(&zip_info);
        if (code != Z_OK)
          status=MagickFalse;
        break;
      }
#endif
      default:
        break;
    }
    quantum_info=DestroyQuantumInfo(quantum_info);
    compress_pixels=(unsigned char *) RelinquishMagickMemory(compress_pixels);
    if (GetNextImageInList(image) == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,SaveImagesTag,scene++,imageListLength);
    if (status == MagickFalse)
      break;
  } while (image_info->adjoin != MagickFalse);
  (void) CloseBlob(image);
  return(status);
}