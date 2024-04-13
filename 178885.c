static MagickBooleanType WriteSUNImage(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
#define RMT_EQUAL_RGB  1
#define RMT_NONE  0
#define RMT_RAW  2
#define RT_STANDARD  1
#define RT_FORMAT_RGB  3

  typedef struct _SUNInfo
  {
    unsigned int
      magic,
      width,
      height,
      depth,
      length,
      type,
      maptype,
      maplength;
  } SUNInfo;

  MagickBooleanType
    status;

  MagickOffsetType
    scene;

  MagickSizeType
    number_pixels;

  register const Quantum
    *p;

  register ssize_t
    i,
    x;

  ssize_t
    y;

  SUNInfo
    sun_info;

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
  do
  {
    /*
      Initialize SUN raster file header.
    */
    (void) TransformImageColorspace(image,sRGBColorspace,exception);
    sun_info.magic=0x59a66a95;
    if ((image->columns != (unsigned int) image->columns) ||
        (image->rows != (unsigned int) image->rows))
      ThrowWriterException(ImageError,"WidthOrHeightExceedsLimit");
    sun_info.width=(unsigned int) image->columns;
    sun_info.height=(unsigned int) image->rows;
    sun_info.type=(unsigned int)
      (image->storage_class == DirectClass ? RT_FORMAT_RGB : RT_STANDARD);
    sun_info.maptype=RMT_NONE;
    sun_info.maplength=0;
    number_pixels=(MagickSizeType) image->columns*image->rows;
    if ((4*number_pixels) != (size_t) (4*number_pixels))
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    if (image->storage_class == DirectClass)
      {
        /*
          Full color SUN raster.
        */
        sun_info.depth=(unsigned int) image->alpha_trait !=
          UndefinedPixelTrait ? 32U : 24U;
        sun_info.length=(unsigned int) ((image->alpha_trait !=
          UndefinedPixelTrait ? 4 : 3)*number_pixels);
        sun_info.length+=sun_info.length & 0x01 ? (unsigned int) image->rows :
          0;
      }
    else
      if (SetImageMonochrome(image,exception) != MagickFalse)
        {
          /*
            Monochrome SUN raster.
          */
          sun_info.depth=1;
          sun_info.length=(unsigned int) (((image->columns+7) >> 3)*
            image->rows);
          sun_info.length+=(unsigned int) (((image->columns/8)+(image->columns %
            8 ? 1 : 0)) % 2 ? image->rows : 0);
        }
      else
        {
          /*
            Colormapped SUN raster.
          */
          sun_info.depth=8;
          sun_info.length=(unsigned int) number_pixels;
          sun_info.length+=(unsigned int) (image->columns & 0x01 ? image->rows :
            0);
          sun_info.maptype=RMT_EQUAL_RGB;
          sun_info.maplength=(unsigned int) (3*image->colors);
        }
    /*
      Write SUN header.
    */
    (void) WriteBlobMSBLong(image,sun_info.magic);
    (void) WriteBlobMSBLong(image,sun_info.width);
    (void) WriteBlobMSBLong(image,sun_info.height);
    (void) WriteBlobMSBLong(image,sun_info.depth);
    (void) WriteBlobMSBLong(image,sun_info.length);
    (void) WriteBlobMSBLong(image,sun_info.type);
    (void) WriteBlobMSBLong(image,sun_info.maptype);
    (void) WriteBlobMSBLong(image,sun_info.maplength);
    /*
      Convert MIFF to SUN raster pixels.
    */
    x=0;
    y=0;
    if (image->storage_class == DirectClass)
      {
        register unsigned char
          *q;

        size_t
          bytes_per_pixel,
          length;

        unsigned char
          *pixels;

        /*
          Allocate memory for pixels.
        */
        bytes_per_pixel=3;
        if (image->alpha_trait != UndefinedPixelTrait)
          bytes_per_pixel++;
        length=image->columns;
        pixels=(unsigned char *) AcquireQuantumMemory(length,4*sizeof(*pixels));
        if (pixels == (unsigned char *) NULL)
          ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
        /*
          Convert DirectClass packet to SUN RGB pixel.
        */
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          p=GetVirtualPixels(image,0,y,image->columns,1,exception);
          if (p == (const Quantum *) NULL)
            break;
          q=pixels;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            if (image->alpha_trait != UndefinedPixelTrait)
              *q++=ScaleQuantumToChar(GetPixelAlpha(image,p));
            *q++=ScaleQuantumToChar(GetPixelRed(image,p));
            *q++=ScaleQuantumToChar(GetPixelGreen(image,p));
            *q++=ScaleQuantumToChar(GetPixelBlue(image,p));
            p+=GetPixelChannels(image);
          }
          if (((bytes_per_pixel*image->columns) & 0x01) != 0)
            *q++='\0';  /* pad scanline */
          (void) WriteBlob(image,(size_t) (q-pixels),pixels);
          if (image->previous == (Image *) NULL)
            {
              status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
              if (status == MagickFalse)
                break;
            }
        }
        pixels=(unsigned char *) RelinquishMagickMemory(pixels);
      }
    else
      if (SetImageMonochrome(image,exception) != MagickFalse)
        {
          register unsigned char
            bit,
            byte;

          /*
            Convert PseudoClass image to a SUN monochrome image.
          */
          (void) SetImageType(image,BilevelType,exception);
          for (y=0; y < (ssize_t) image->rows; y++)
          {
            p=GetVirtualPixels(image,0,y,image->columns,1,exception);
            if (p == (const Quantum *) NULL)
              break;
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
                  (void) WriteBlobByte(image,byte);
                  bit=0;
                  byte=0;
                }
              p+=GetPixelChannels(image);
            }
            if (bit != 0)
              (void) WriteBlobByte(image,(unsigned char) (byte << (8-bit)));
            if ((((image->columns/8)+
                (image->columns % 8 ? 1 : 0)) % 2) != 0)
              (void) WriteBlobByte(image,0);  /* pad scanline */
            if (image->previous == (Image *) NULL)
              {
                status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                image->rows);
                if (status == MagickFalse)
                  break;
              }
          }
        }
      else
        {
          /*
            Dump colormap to file.
          */
          for (i=0; i < (ssize_t) image->colors; i++)
            (void) WriteBlobByte(image,ScaleQuantumToChar(
              ClampToQuantum(image->colormap[i].red)));
          for (i=0; i < (ssize_t) image->colors; i++)
            (void) WriteBlobByte(image,ScaleQuantumToChar(
              ClampToQuantum(image->colormap[i].green)));
          for (i=0; i < (ssize_t) image->colors; i++)
            (void) WriteBlobByte(image,ScaleQuantumToChar(
              ClampToQuantum(image->colormap[i].blue)));
          /*
            Convert PseudoClass packet to SUN colormapped pixel.
          */
          for (y=0; y < (ssize_t) image->rows; y++)
          {
            p=GetVirtualPixels(image,0,y,image->columns,1,exception);
            if (p == (const Quantum *) NULL)
              break;
            for (x=0; x < (ssize_t) image->columns; x++)
            {
              (void) WriteBlobByte(image,(unsigned char)
                GetPixelIndex(image,p));
              p+=GetPixelChannels(image);
            }
            if (image->columns & 0x01)
              (void) WriteBlobByte(image,0);  /* pad scanline */
            if (image->previous == (Image *) NULL)
              {
                status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
                  image->rows);
                if (status == MagickFalse)
                  break;
              }
          }
        }
    if (GetNextImageInList(image) == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,SaveImagesTag,scene++,
      GetImageListLength(image));
    if (status == MagickFalse)
      break;
  } while (image_info->adjoin != MagickFalse);
  (void) CloseBlob(image);
  return(MagickTrue);
}