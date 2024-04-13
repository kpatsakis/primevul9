static MagickBooleanType WritePCLImage(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
  char
    buffer[MagickPathExtent];

  CompressionType
    compression;

  const char
    *option;

  MagickBooleanType
    status;

  MagickOffsetType
    scene;

  register const Quantum *p;

  register ssize_t i, x;

  register unsigned char *q;

  size_t
    density,
    imageListLength,
    length,
    one,
    packets;

  ssize_t
    y;

  unsigned char
    bits_per_pixel,
    *compress_pixels,
    *pixels,
    *previous_pixels;

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
  density=75;
  if (image_info->density != (char *) NULL)
    {
      GeometryInfo
        geometry;

      (void) ParseGeometry(image_info->density,&geometry);
      density=(size_t) geometry.rho;
    }
  scene=0;
  one=1;
  imageListLength=GetImageListLength(image);
  do
  {
    /*
      Initialize the printer.
    */
    (void) TransformImageColorspace(image,sRGBColorspace,exception);
    (void) WriteBlobString(image,"\033E");  /* printer reset */
    (void) WriteBlobString(image,"\033*r3F");  /* set presentation mode */
    (void) FormatLocaleString(buffer,MagickPathExtent,"\033*r%.20gs%.20gT",
      (double) image->columns,(double) image->rows);
    (void) WriteBlobString(image,buffer);
    (void) FormatLocaleString(buffer,MagickPathExtent,"\033*t%.20gR",(double)
      density);
    (void) WriteBlobString(image,buffer);
    (void) WriteBlobString(image,"\033&l0E");  /* top margin 0 */
    if (SetImageMonochrome(image,exception) != MagickFalse)
      {
        /*
          Monochrome image: use default printer monochrome setup.
        */
        bits_per_pixel=1;
      }
    else
      if (image->storage_class == DirectClass)
        {
          /*
            DirectClass image.
          */
          bits_per_pixel=24;
          (void) WriteBlobString(image,"\033*v6W"); /* set color mode */
          (void) WriteBlobByte(image,0); /* RGB */
          (void) WriteBlobByte(image,3); /* direct by pixel */
          (void) WriteBlobByte(image,0); /* bits per index (ignored) */
          (void) WriteBlobByte(image,8); /* bits per red component */
          (void) WriteBlobByte(image,8); /* bits per green component */
          (void) WriteBlobByte(image,8); /* bits per blue component */
        }
      else
        {
          /*
            Colormapped image.
          */
          bits_per_pixel=8;
          (void) WriteBlobString(image,"\033*v6W"); /* set color mode... */
          (void) WriteBlobByte(image,0); /* RGB */
          (void) WriteBlobByte(image,1); /* indexed by pixel */
          (void) WriteBlobByte(image,bits_per_pixel); /* bits per index */
          (void) WriteBlobByte(image,8); /* bits per red component */
          (void) WriteBlobByte(image,8); /* bits per green component */
          (void) WriteBlobByte(image,8); /* bits per blue component */
          for (i=0; i < (ssize_t) image->colors; i++)
          {
            (void) FormatLocaleString(buffer,MagickPathExtent,
              "\033*v%da%db%dc%.20gI",
              ScaleQuantumToChar(image->colormap[i].red),
              ScaleQuantumToChar(image->colormap[i].green),
              ScaleQuantumToChar(image->colormap[i].blue),(double) i);
            (void) WriteBlobString(image,buffer);
          }
          for (one=1; i < (ssize_t) (one << bits_per_pixel); i++)
          {
            (void) FormatLocaleString(buffer,MagickPathExtent,"\033*v%.20gI",
              (double) i);
            (void) WriteBlobString(image,buffer);
          }
        }
    option=GetImageOption(image_info,"pcl:fit-to-page");
    if (IsStringTrue(option) != MagickFalse)
      (void) WriteBlobString(image,"\033*r3A");
    else
      (void) WriteBlobString(image,"\033*r1A");  /* start raster graphics */
    (void) WriteBlobString(image,"\033*b0Y");  /* set y offset */
    length=(image->columns*bits_per_pixel+7)/8;
    pixels=(unsigned char *) AcquireQuantumMemory(length+1,sizeof(*pixels));
    if (pixels == (unsigned char *) NULL)
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    (void) memset(pixels,0,(length+1)*sizeof(*pixels));
    compress_pixels=(unsigned char *) NULL;
    previous_pixels=(unsigned char *) NULL;

    compression=UndefinedCompression;
    if (image_info->compression != UndefinedCompression)
      compression=image_info->compression;
    switch (compression)
    {
      case NoCompression:
      {
        (void) FormatLocaleString(buffer,MagickPathExtent,"\033*b0M");
        (void) WriteBlobString(image,buffer);
        break;
      }
      case RLECompression:
      {
        compress_pixels=(unsigned char *) AcquireQuantumMemory(length+256,
          sizeof(*compress_pixels));
        if (compress_pixels == (unsigned char *) NULL)
          {
            pixels=(unsigned char *) RelinquishMagickMemory(pixels);
            ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
          }
        (void) memset(compress_pixels,0,(length+256)*
          sizeof(*compress_pixels));
        (void) FormatLocaleString(buffer,MagickPathExtent,"\033*b2M");
        (void) WriteBlobString(image,buffer);
        break;
      }
      default:
      {
        compress_pixels=(unsigned char *) AcquireQuantumMemory(3*length+256,
          sizeof(*compress_pixels));
        if (compress_pixels == (unsigned char *) NULL)
          {
            pixels=(unsigned char *) RelinquishMagickMemory(pixels);
            ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
          }
        (void) memset(compress_pixels,0,(3*length+256)*
          sizeof(*compress_pixels));
        previous_pixels=(unsigned char *) AcquireQuantumMemory(length+1,
          sizeof(*previous_pixels));
        if (previous_pixels == (unsigned char *) NULL)
          {
            compress_pixels=(unsigned char *) RelinquishMagickMemory(
              compress_pixels);
            pixels=(unsigned char *) RelinquishMagickMemory(pixels);
            ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
          }
        (void) memset(previous_pixels,0,(length+1)*
          sizeof(*previous_pixels));
        (void) FormatLocaleString(buffer,MagickPathExtent,"\033*b3M");
        (void) WriteBlobString(image,buffer);
        break;
      }
    }
    for (y=0; y < (ssize_t) image->rows; y++)
    {
      p=GetVirtualPixels(image,0,y,image->columns,1,exception);
      if (p == (const Quantum *) NULL)
        break;
      q=pixels;
      switch (bits_per_pixel)
      {
        case 1:
        {
          register unsigned char
            bit,
            byte;

          /*
            Monochrome image.
          */
          bit=0;
          byte=0;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            byte<<=1;
            if (GetPixelLuma(image,p) < (QuantumRange/2.0))
              byte|=0x01;
            bit++;
            if (bit == 8)
              {
                *q++=byte;
                bit=0;
                byte=0;
              }
            p+=GetPixelChannels(image);
          }
          if (bit != 0)
            *q++=byte << (8-bit);
          break;
        }
        case 8:
        {
          /*
            Colormapped image.
          */
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            *q++=(unsigned char) GetPixelIndex(image,p);
            p+=GetPixelChannels(image);
          }
          break;
        }
        case 24:
        case 32:
        {
          /*
            Truecolor image.
          */
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            *q++=ScaleQuantumToChar(GetPixelRed(image,p));
            *q++=ScaleQuantumToChar(GetPixelGreen(image,p));
            *q++=ScaleQuantumToChar(GetPixelBlue(image,p));
            p+=GetPixelChannels(image);
          }
          break;
        }
      }
      switch (compression)
      {
        case NoCompression:
        {
          (void) FormatLocaleString(buffer,MagickPathExtent,"\033*b%.20gW",
            (double) length);
          (void) WriteBlobString(image,buffer);
          (void) WriteBlob(image,length,pixels);
          break;
        }
        case RLECompression:
        {
          packets=PCLPackbitsCompressImage(length,pixels,compress_pixels);
          (void) FormatLocaleString(buffer,MagickPathExtent,"\033*b%.20gW",
            (double) packets);
          (void) WriteBlobString(image,buffer);
          (void) WriteBlob(image,packets,compress_pixels);
          break;
        }
        default:
        {
          if (y == 0)
            for (i=0; i < (ssize_t) length; i++)
              previous_pixels[i]=(~pixels[i]);
          packets=PCLDeltaCompressImage(length,previous_pixels,pixels,
            compress_pixels);
          (void) FormatLocaleString(buffer,MagickPathExtent,"\033*b%.20gW",
            (double) packets);
          (void) WriteBlobString(image,buffer);
          (void) WriteBlob(image,packets,compress_pixels);
          (void) memcpy(previous_pixels,pixels,length*
            sizeof(*pixels));
          break;
        }
      }
    }
    (void) WriteBlobString(image,"\033*rB");  /* end graphics */
    switch (compression)
    {
      case NoCompression:
        break;
      case RLECompression:
      {
        compress_pixels=(unsigned char *) RelinquishMagickMemory(
          compress_pixels);
        break;
      }
      default:
      {
        previous_pixels=(unsigned char *) RelinquishMagickMemory(
          previous_pixels);
        compress_pixels=(unsigned char *) RelinquishMagickMemory(
          compress_pixels);
        break;
      }
    }
    pixels=(unsigned char *) RelinquishMagickMemory(pixels);
    if (GetNextImageInList(image) == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,SaveImagesTag,scene++,imageListLength);
    if (status == MagickFalse)
      break;
  } while (image_info->adjoin != MagickFalse);
  (void) WriteBlobString(image,"\033E");
  (void) CloseBlob(image);
  return(MagickTrue);
}