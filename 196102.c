static MagickBooleanType WriteICONImage(const ImageInfo *image_info,
  Image *image,ExceptionInfo *exception)
{
  const char
    *option;

  IconFile
    icon_file;

  IconInfo
    icon_info;

  Image
    *images,
    *next;
  
  MagickBooleanType
    status;

  MagickOffsetType
    offset,
    scene;

  register const Quantum
    *p;

  register ssize_t
    i,
    x;

  register unsigned char
    *q;

  size_t
    bytes_per_line,
    scanline_pad;

  ssize_t
    y;

  unsigned char
    bit,
    byte,
    *pixels;

  /*
    Open output image file.
  */
  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
    (void) LogMagickEvent(CoderEvent,GetMagickModule(),"%s",image->filename);
  assert(exception != (ExceptionInfo *) NULL);
  assert(exception->signature == MagickCoreSignature);
  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  images=(Image *) NULL;
  option=GetImageOption(image_info,"icon:auto-resize");
  if (option != (const char *) NULL)
    {
      images=AutoResizeImage(image,option,&scene,exception);
      if (images == (Image *) NULL)
        ThrowWriterException(ImageError,"InvalidDimensions");
    }
  else
    {
      scene=0;
      next=image;
      do
      {
        if ((image->columns > 256L) || (image->rows > 256L))
          ThrowWriterException(ImageError,"WidthOrHeightExceedsLimit");
        scene++;
        next=SyncNextImageInList(next);
      } while ((next != (Image *) NULL) && (image_info->adjoin != MagickFalse));
    }
  /*
    Dump out a ICON header template to be properly initialized later.
  */
  (void) WriteBlobLSBShort(image,0);
  (void) WriteBlobLSBShort(image,1);
  (void) WriteBlobLSBShort(image,(unsigned char) scene);
  (void) ResetMagickMemory(&icon_file,0,sizeof(icon_file));
  (void) ResetMagickMemory(&icon_info,0,sizeof(icon_info));
  scene=0;
  next=(images != (Image *) NULL) ? images : image;
  do
  {
    (void) WriteBlobByte(image,icon_file.directory[scene].width);
    (void) WriteBlobByte(image,icon_file.directory[scene].height);
    (void) WriteBlobByte(image,icon_file.directory[scene].colors);
    (void) WriteBlobByte(image,icon_file.directory[scene].reserved);
    (void) WriteBlobLSBShort(image,icon_file.directory[scene].planes);
    (void) WriteBlobLSBShort(image,icon_file.directory[scene].bits_per_pixel);
    (void) WriteBlobLSBLong(image,(unsigned int)
      icon_file.directory[scene].size);
    (void) WriteBlobLSBLong(image,(unsigned int)
      icon_file.directory[scene].offset);
    scene++;
    next=SyncNextImageInList(next);
  } while ((next != (Image *) NULL) && (image_info->adjoin != MagickFalse));
  scene=0;
  next=(images != (Image *) NULL) ? images : image;
  do
  {
    if ((next->columns > 255L) && (next->rows > 255L) &&
        ((next->compression == UndefinedCompression) ||
        (next->compression == ZipCompression)))
      {
        Image
          *write_image;

        ImageInfo
          *write_info;

        size_t
          length;

        unsigned char
          *png;

        write_image=CloneImage(next,0,0,MagickTrue,exception);
        if (write_image == (Image *) NULL)
          {
            images=DestroyImageList(images);
            return(MagickFalse);
          }
        write_info=CloneImageInfo(image_info);
        (void) CopyMagickString(write_info->filename,"PNG:",MagickPathExtent);

        /* Don't write any ancillary chunks except for gAMA */
        (void) SetImageArtifact(write_image,"png:include-chunk","none,gama");

        /* Only write PNG32 formatted PNG (32-bit RGBA), 8 bits per channel */
        (void) SetImageArtifact(write_image,"png:format","png32");

        png=(unsigned char *) ImageToBlob(write_info,write_image,&length,
          exception);
        write_image=DestroyImageList(write_image);
        write_info=DestroyImageInfo(write_info);
        if (png == (unsigned char *) NULL)
          {
            images=DestroyImageList(images);
            return(MagickFalse);
          }
        icon_file.directory[scene].width=0;
        icon_file.directory[scene].height=0;
        icon_file.directory[scene].colors=0;
        icon_file.directory[scene].reserved=0;
        icon_file.directory[scene].planes=1;
        icon_file.directory[scene].bits_per_pixel=32;
        icon_file.directory[scene].size=(size_t) length;
        icon_file.directory[scene].offset=(size_t) TellBlob(image);
        (void) WriteBlob(image,(size_t) length,png);
        png=(unsigned char *) RelinquishMagickMemory(png);
      }
    else
      {
        /*
          Initialize ICON raster file header.
        */
        (void) TransformImageColorspace(next,sRGBColorspace,exception);
        icon_info.file_size=14+12+28;
        icon_info.offset_bits=icon_info.file_size;
        icon_info.compression=BI_RGB;
        if ((next->storage_class != DirectClass) && (next->colors > 256))
          (void) SetImageStorageClass(next,DirectClass,exception);
        if (next->storage_class == DirectClass)
          {
            /*
              Full color ICON raster.
            */
            icon_info.number_colors=0;
            icon_info.bits_per_pixel=32;
            icon_info.compression=(size_t) BI_RGB;
          }
        else
          {
            size_t
              one;

            /*
              Colormapped ICON raster.
            */
            icon_info.bits_per_pixel=8;
            if (next->colors <= 256)
              icon_info.bits_per_pixel=8;
            if (next->colors <= 16)
              icon_info.bits_per_pixel=4;
            if (next->colors <= 2)
              icon_info.bits_per_pixel=1;
            one=1;
            icon_info.number_colors=one << icon_info.bits_per_pixel;
            if (icon_info.number_colors < next->colors)
              {
                (void) SetImageStorageClass(next,DirectClass,exception);
                icon_info.number_colors=0;
                icon_info.bits_per_pixel=(unsigned short) 24;
                icon_info.compression=(size_t) BI_RGB;
              }
            else
              {
                size_t
                  one;

                one=1;
                icon_info.file_size+=3*(one << icon_info.bits_per_pixel);
                icon_info.offset_bits+=3*(one << icon_info.bits_per_pixel);
                icon_info.file_size+=(one << icon_info.bits_per_pixel);
                icon_info.offset_bits+=(one << icon_info.bits_per_pixel);
              }
          }
        bytes_per_line=(((next->columns*icon_info.bits_per_pixel)+31) &
          ~31) >> 3;
        icon_info.ba_offset=0;
        icon_info.width=(ssize_t) next->columns;
        icon_info.height=(ssize_t) next->rows;
        icon_info.planes=1;
        icon_info.image_size=bytes_per_line*next->rows;
        icon_info.size=40;
        icon_info.size+=(4*icon_info.number_colors);
        icon_info.size+=icon_info.image_size;
        icon_info.size+=(((icon_info.width+31) & ~31) >> 3)*icon_info.height;
        icon_info.file_size+=icon_info.image_size;
        icon_info.x_pixels=0;
        icon_info.y_pixels=0;
        switch (next->units)
        {
          case UndefinedResolution:
          case PixelsPerInchResolution:
          {
            icon_info.x_pixels=(size_t) (100.0*next->resolution.x/2.54);
            icon_info.y_pixels=(size_t) (100.0*next->resolution.y/2.54);
            break;
          }
          case PixelsPerCentimeterResolution:
          {
            icon_info.x_pixels=(size_t) (100.0*next->resolution.x);
            icon_info.y_pixels=(size_t) (100.0*next->resolution.y);
            break;
          }
        }
        icon_info.colors_important=icon_info.number_colors;
        /*
          Convert MIFF to ICON raster pixels.
        */
        pixels=(unsigned char *) AcquireQuantumMemory((size_t)
          icon_info.image_size,sizeof(*pixels));
        if (pixels == (unsigned char *) NULL)
          {
            images=DestroyImageList(images);
            ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
          }
        (void) ResetMagickMemory(pixels,0,(size_t) icon_info.image_size);
        switch (icon_info.bits_per_pixel)
        {
          case 1:
          {
            size_t
              bit,
              byte;

            /*
              Convert PseudoClass image to a ICON monochrome image.
            */
            for (y=0; y < (ssize_t) next->rows; y++)
            {
              p=GetVirtualPixels(next,0,y,next->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              q=pixels+(next->rows-y-1)*bytes_per_line;
              bit=0;
              byte=0;
              for (x=0; x < (ssize_t) next->columns; x++)
              {
                byte<<=1;
                byte|=GetPixelIndex(next,p) != 0 ? 0x01 : 0x00;
                bit++;
                if (bit == 8)
                  {
                    *q++=(unsigned char) byte;
                    bit=0;
                    byte=0;
                  }
                p+=GetPixelChannels(image);
              }
              if (bit != 0)
                *q++=(unsigned char) (byte << (8-bit));
              if (next->previous == (Image *) NULL)
                {
                  status=SetImageProgress(next,SaveImageTag,y,next->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          case 4:
          {
            size_t
              nibble,
              byte;

            /*
              Convert PseudoClass image to a ICON monochrome image.
            */
            for (y=0; y < (ssize_t) next->rows; y++)
            {
              p=GetVirtualPixels(next,0,y,next->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              q=pixels+(next->rows-y-1)*bytes_per_line;
              nibble=0;
              byte=0;
              for (x=0; x < (ssize_t) next->columns; x++)
              {
                byte<<=4;
                byte|=((size_t) GetPixelIndex(next,p) & 0x0f);
                nibble++;
                if (nibble == 2)
                  {
                    *q++=(unsigned char) byte;
                    nibble=0;
                    byte=0;
                  }
                p+=GetPixelChannels(image);
              }
              if (nibble != 0)
                *q++=(unsigned char) (byte << 4);
              if (next->previous == (Image *) NULL)
                {
                  status=SetImageProgress(next,SaveImageTag,y,next->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          case 8:
          {
            /*
              Convert PseudoClass packet to ICON pixel.
            */
            for (y=0; y < (ssize_t) next->rows; y++)
            {
              p=GetVirtualPixels(next,0,y,next->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              q=pixels+(next->rows-y-1)*bytes_per_line;
              for (x=0; x < (ssize_t) next->columns; x++)
              {
                *q++=(unsigned char) GetPixelIndex(next,p);
                p+=GetPixelChannels(image);
              }
              if (next->previous == (Image *) NULL)
                {
                  status=SetImageProgress(next,SaveImageTag,y,next->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
          case 24:
          case 32:
          {
            /*
              Convert DirectClass packet to ICON BGR888 or BGRA8888 pixel.
            */
            for (y=0; y < (ssize_t) next->rows; y++)
            {
              p=GetVirtualPixels(next,0,y,next->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              q=pixels+(next->rows-y-1)*bytes_per_line;
              for (x=0; x < (ssize_t) next->columns; x++)
              {
                *q++=ScaleQuantumToChar(GetPixelBlue(next,p));
                *q++=ScaleQuantumToChar(GetPixelGreen(next,p));
                *q++=ScaleQuantumToChar(GetPixelRed(next,p));
                if (next->alpha_trait == UndefinedPixelTrait)
                  *q++=ScaleQuantumToChar(QuantumRange);
                else
                  *q++=ScaleQuantumToChar(GetPixelAlpha(next,p));
                p+=GetPixelChannels(next);
              }
              if (icon_info.bits_per_pixel == 24)
                for (x=3L*(ssize_t) next->columns; x < (ssize_t) bytes_per_line; x++)
                  *q++=0x00;
              if (next->previous == (Image *) NULL)
                {
                  status=SetImageProgress(next,SaveImageTag,y,next->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
            break;
          }
        }
        /*
          Write 40-byte version 3+ bitmap header.
        */
        icon_file.directory[scene].width=(unsigned char) icon_info.width;
        icon_file.directory[scene].height=(unsigned char) icon_info.height;
        icon_file.directory[scene].colors=(unsigned char)
          icon_info.number_colors;
        icon_file.directory[scene].reserved=0;
        icon_file.directory[scene].planes=icon_info.planes;
        icon_file.directory[scene].bits_per_pixel=icon_info.bits_per_pixel;
        icon_file.directory[scene].size=icon_info.size;
        icon_file.directory[scene].offset=(size_t) TellBlob(image);
        (void) WriteBlobLSBLong(image,(unsigned int) 40);
        (void) WriteBlobLSBLong(image,(unsigned int) icon_info.width);
        (void) WriteBlobLSBLong(image,(unsigned int) icon_info.height*2);
        (void) WriteBlobLSBShort(image,icon_info.planes);
        (void) WriteBlobLSBShort(image,icon_info.bits_per_pixel);
        (void) WriteBlobLSBLong(image,(unsigned int) icon_info.compression);
        (void) WriteBlobLSBLong(image,(unsigned int) icon_info.image_size);
        (void) WriteBlobLSBLong(image,(unsigned int) icon_info.x_pixels);
        (void) WriteBlobLSBLong(image,(unsigned int) icon_info.y_pixels);
        (void) WriteBlobLSBLong(image,(unsigned int) icon_info.number_colors);
        (void) WriteBlobLSBLong(image,(unsigned int) icon_info.colors_important);
        if (next->storage_class == PseudoClass)
          {
            unsigned char
              *icon_colormap;

            /*
              Dump colormap to file.
            */
            icon_colormap=(unsigned char *) AcquireQuantumMemory((size_t)
              (1UL << icon_info.bits_per_pixel),4UL*sizeof(*icon_colormap));
            if (icon_colormap == (unsigned char *) NULL)
              {
                images=DestroyImageList(images);
                ThrowWriterException(ResourceLimitError,
                  "MemoryAllocationFailed");
              }
            q=icon_colormap;
            for (i=0; i < (ssize_t) next->colors; i++)
            {
              *q++=ScaleQuantumToChar(next->colormap[i].blue);
              *q++=ScaleQuantumToChar(next->colormap[i].green);
              *q++=ScaleQuantumToChar(next->colormap[i].red);
              *q++=(unsigned char) 0x0;
            }
            for ( ; i < (ssize_t) (1UL << icon_info.bits_per_pixel); i++)
            {
              *q++=(unsigned char) 0x00;
              *q++=(unsigned char) 0x00;
              *q++=(unsigned char) 0x00;
              *q++=(unsigned char) 0x00;
            }
            (void) WriteBlob(image,(size_t) (4UL*(1UL <<
              icon_info.bits_per_pixel)),icon_colormap);
            icon_colormap=(unsigned char *) RelinquishMagickMemory(
              icon_colormap);
          }
        (void) WriteBlob(image,(size_t) icon_info.image_size,pixels);
        pixels=(unsigned char *) RelinquishMagickMemory(pixels);
        /*
          Write matte mask.
        */
        scanline_pad=(((next->columns+31) & ~31)-next->columns) >> 3;
        for (y=((ssize_t) next->rows - 1); y >= 0; y--)
        {
          p=GetVirtualPixels(next,0,y,next->columns,1,exception);
          if (p == (const Quantum *) NULL)
            break;
          bit=0;
          byte=0;
          for (x=0; x < (ssize_t) next->columns; x++)
          {
            byte<<=1;
            if ((next->alpha_trait != UndefinedPixelTrait) &&
                (GetPixelAlpha(next,p) == (Quantum) TransparentAlpha))
              byte|=0x01;
            bit++;
            if (bit == 8)
              {
                (void) WriteBlobByte(image,(unsigned char) byte);
                bit=0;
                byte=0;
              }
            p+=GetPixelChannels(next);
          }
          if (bit != 0)
            (void) WriteBlobByte(image,(unsigned char) (byte << (8-bit)));
          for (i=0; i < (ssize_t) scanline_pad; i++)
            (void) WriteBlobByte(image,(unsigned char) 0);
        }
      }
    if (GetNextImageInList(next) == (Image *) NULL)
      break;
    status=SetImageProgress(next,SaveImagesTag,scene++,
      GetImageListLength(next));
    if (status == MagickFalse)
      break;
    next=SyncNextImageInList(next);
  } while ((next != (Image *) NULL) && (image_info->adjoin != MagickFalse));
  offset=SeekBlob(image,0,SEEK_SET);
  (void) offset;
  (void) WriteBlobLSBShort(image,0);
  (void) WriteBlobLSBShort(image,1);
  (void) WriteBlobLSBShort(image,(unsigned short) (scene+1));
  scene=0;
  next=(images != (Image *) NULL) ? images : image;
  do
  {
    (void) WriteBlobByte(image,icon_file.directory[scene].width);
    (void) WriteBlobByte(image,icon_file.directory[scene].height);
    (void) WriteBlobByte(image,icon_file.directory[scene].colors);
    (void) WriteBlobByte(image,icon_file.directory[scene].reserved);
    (void) WriteBlobLSBShort(image,icon_file.directory[scene].planes);
    (void) WriteBlobLSBShort(image,icon_file.directory[scene].bits_per_pixel);
    (void) WriteBlobLSBLong(image,(unsigned int)
      icon_file.directory[scene].size);
    (void) WriteBlobLSBLong(image,(unsigned int)
      icon_file.directory[scene].offset);
    scene++;
    next=SyncNextImageInList(next);
  } while ((next != (Image *) NULL) && (image_info->adjoin != MagickFalse));
  (void) CloseBlob(image);
  images=DestroyImageList(images);
  return(MagickTrue);
}