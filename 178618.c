static MagickBooleanType WriteVIFFImage(const ImageInfo *image_info,
  Image *image,ExceptionInfo *exception)
{
#define VFF_CM_genericRGB  15
#define VFF_CM_NONE  0
#define VFF_DEP_IEEEORDER  0x2
#define VFF_DES_RAW  0
#define VFF_LOC_IMPLICIT  1
#define VFF_MAPTYP_NONE  0
#define VFF_MAPTYP_1_BYTE  1
#define VFF_MS_NONE  0
#define VFF_MS_ONEPERBAND  1
#define VFF_TYP_BIT  0
#define VFF_TYP_1_BYTE  1

  typedef struct _ViffInfo
  {
    char
      identifier,
      file_type,
      release,
      version,
      machine_dependency,
      reserve[3],
      comment[512];

    size_t
      rows,
      columns,
      subrows;

    int
      x_offset,
      y_offset;

    unsigned int
      x_bits_per_pixel,
      y_bits_per_pixel,
      location_type,
      location_dimension,
      number_of_images,
      number_data_bands,
      data_storage_type,
      data_encode_scheme,
      map_scheme,
      map_storage_type,
      map_rows,
      map_columns,
      map_subrows,
      map_enable,
      maps_per_cycle,
      color_space_model;
  } ViffInfo;

  const char
    *value;

  MagickBooleanType
    status;

  MagickOffsetType
    scene;

  MagickSizeType
    number_pixels,
    packets;

  MemoryInfo
    *pixel_info;

  register const Quantum
    *p;

  register ssize_t
    x;

  register ssize_t
    i;

  register unsigned char
    *q;

  size_t
    imageListLength;

  ssize_t
    y;

  unsigned char
    *pixels;

  ViffInfo
    viff_info;

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
  (void) memset(&viff_info,0,sizeof(ViffInfo));
  scene=0;
  imageListLength=GetImageListLength(image);
  do
  {
    /*
      Initialize VIFF image structure.
    */
    (void) TransformImageColorspace(image,sRGBColorspace,exception);
DisableMSCWarning(4310)
    viff_info.identifier=(char) 0xab;
RestoreMSCWarning
    viff_info.file_type=1;
    viff_info.release=1;
    viff_info.version=3;
    viff_info.machine_dependency=VFF_DEP_IEEEORDER;  /* IEEE byte ordering */
    *viff_info.comment='\0';
    value=GetImageProperty(image,"comment",exception);
    if (value != (const char *) NULL)
      (void) CopyMagickString(viff_info.comment,value,MagickMin(strlen(value),
        511)+1);
    viff_info.rows=image->columns;
    viff_info.columns=image->rows;
    viff_info.subrows=0;
    viff_info.x_offset=(~0);
    viff_info.y_offset=(~0);
    viff_info.x_bits_per_pixel=0;
    viff_info.y_bits_per_pixel=0;
    viff_info.location_type=VFF_LOC_IMPLICIT;
    viff_info.location_dimension=0;
    viff_info.number_of_images=1;
    viff_info.data_encode_scheme=VFF_DES_RAW;
    viff_info.map_scheme=VFF_MS_NONE;
    viff_info.map_storage_type=VFF_MAPTYP_NONE;
    viff_info.map_rows=0;
    viff_info.map_columns=0;
    viff_info.map_subrows=0;
    viff_info.map_enable=1;  /* no colormap */
    viff_info.maps_per_cycle=0;
    number_pixels=(MagickSizeType) image->columns*image->rows;
    if (image->storage_class == DirectClass)
      {
        /*
          Full color VIFF raster.
        */
        viff_info.number_data_bands=image->alpha_trait ? 4U : 3U;
        viff_info.color_space_model=VFF_CM_genericRGB;
        viff_info.data_storage_type=VFF_TYP_1_BYTE;
        packets=viff_info.number_data_bands*number_pixels;
      }
    else
      {
        viff_info.number_data_bands=1;
        viff_info.color_space_model=VFF_CM_NONE;
        viff_info.data_storage_type=VFF_TYP_1_BYTE;
        packets=number_pixels;
        if (SetImageGray(image,exception) == MagickFalse)
          {
            /*
              Colormapped VIFF raster.
            */
            viff_info.map_scheme=VFF_MS_ONEPERBAND;
            viff_info.map_storage_type=VFF_MAPTYP_1_BYTE;
            viff_info.map_rows=3;
            viff_info.map_columns=(unsigned int) image->colors;
          }
        else
          if (image->colors <= 2)
            {
              /*
                Monochrome VIFF raster.
              */
              viff_info.data_storage_type=VFF_TYP_BIT;
              packets=((image->columns+7) >> 3)*image->rows;
            }
      }
    /*
      Write VIFF image header (pad to 1024 bytes).
    */
    (void) WriteBlob(image,sizeof(viff_info.identifier),(unsigned char *)
      &viff_info.identifier);
    (void) WriteBlob(image,sizeof(viff_info.file_type),(unsigned char *)
      &viff_info.file_type);
    (void) WriteBlob(image,sizeof(viff_info.release),(unsigned char *)
      &viff_info.release);
    (void) WriteBlob(image,sizeof(viff_info.version),(unsigned char *)
      &viff_info.version);
    (void) WriteBlob(image,sizeof(viff_info.machine_dependency),
      (unsigned char *) &viff_info.machine_dependency);
    (void) WriteBlob(image,sizeof(viff_info.reserve),(unsigned char *)
      viff_info.reserve);
    (void) WriteBlob(image,512,(unsigned char *) viff_info.comment);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.rows);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.columns);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.subrows);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.x_offset);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.y_offset);
    viff_info.x_bits_per_pixel=(unsigned int) ((63 << 24) | (128 << 16));
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.x_bits_per_pixel);
    viff_info.y_bits_per_pixel=(unsigned int) ((63 << 24) | (128 << 16));
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.y_bits_per_pixel);
    (void) WriteBlobMSBLong(image,viff_info.location_type);
    (void) WriteBlobMSBLong(image,viff_info.location_dimension);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.number_of_images);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.number_data_bands);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.data_storage_type);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.data_encode_scheme);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.map_scheme);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.map_storage_type);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.map_rows);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.map_columns);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.map_subrows);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.map_enable);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.maps_per_cycle);
    (void) WriteBlobMSBLong(image,(unsigned int) viff_info.color_space_model);
    for (i=0; i < 420; i++)
      (void) WriteBlobByte(image,'\0');
    /*
      Convert MIFF to VIFF raster pixels.
    */
    pixel_info=AcquireVirtualMemory((size_t) packets,sizeof(*pixels));
    if (pixel_info == (MemoryInfo *) NULL)
      ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
    pixels=(unsigned char *) GetVirtualMemoryBlob(pixel_info);
    q=pixels;
    if (image->storage_class == DirectClass)
      {
        /*
          Convert DirectClass packet to VIFF RGB pixel.
        */
        number_pixels=(MagickSizeType) image->columns*image->rows;
        for (y=0; y < (ssize_t) image->rows; y++)
        {
          p=GetVirtualPixels(image,0,y,image->columns,1,exception);
          if (p == (const Quantum *) NULL)
            break;
          for (x=0; x < (ssize_t) image->columns; x++)
          {
            *q=ScaleQuantumToChar(GetPixelRed(image,p));
            *(q+number_pixels)=ScaleQuantumToChar(GetPixelGreen(image,p));
            *(q+number_pixels*2)=ScaleQuantumToChar(GetPixelBlue(image,p));
            if (image->alpha_trait != UndefinedPixelTrait)
              *(q+number_pixels*3)=ScaleQuantumToChar((Quantum)
                (GetPixelAlpha(image,p)));
            p+=GetPixelChannels(image);
            q++;
          }
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
      if (SetImageGray(image,exception) == MagickFalse)
        {
          unsigned char
            *viff_colormap;

          /*
            Dump colormap to file.
          */
          viff_colormap=(unsigned char *) AcquireQuantumMemory(image->colors,
            3*sizeof(*viff_colormap));
          if (viff_colormap == (unsigned char *) NULL)
            ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
          q=viff_colormap;
          for (i=0; i < (ssize_t) image->colors; i++)
            *q++=ScaleQuantumToChar(ClampToQuantum(image->colormap[i].red));
          for (i=0; i < (ssize_t) image->colors; i++)
            *q++=ScaleQuantumToChar(ClampToQuantum(image->colormap[i].green));
          for (i=0; i < (ssize_t) image->colors; i++)
            *q++=ScaleQuantumToChar(ClampToQuantum(image->colormap[i].blue));
          (void) WriteBlob(image,3*image->colors,viff_colormap);
          viff_colormap=(unsigned char *) RelinquishMagickMemory(viff_colormap);
          /*
            Convert PseudoClass packet to VIFF colormapped pixels.
          */
          q=pixels;
          for (y=0; y < (ssize_t) image->rows; y++)
          {
            p=GetVirtualPixels(image,0,y,image->columns,1,exception);
            if (p == (const Quantum *) NULL)
              break;
            for (x=0; x < (ssize_t) image->columns; x++)
            {
              *q++=(unsigned char) GetPixelIndex(image,p);
              p+=GetPixelChannels(image);
            }
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
        if (image->colors <= 2)
          {
            ssize_t
              x,
              y;

            register unsigned char
              bit,
              byte;

            /*
              Convert PseudoClass image to a VIFF monochrome image.
            */
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              p=GetVirtualPixels(image,0,y,image->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              bit=0;
              byte=0;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                byte>>=1;
                if (GetPixelLuma(image,p) < (QuantumRange/2.0))
                  byte|=0x80;
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
                *q++=byte >> (8-bit);
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,SaveImageTag,(MagickOffsetType)
                    y,image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
          }
        else
          {
            /*
              Convert PseudoClass packet to VIFF grayscale pixel.
            */
            for (y=0; y < (ssize_t) image->rows; y++)
            {
              p=GetVirtualPixels(image,0,y,image->columns,1,exception);
              if (p == (const Quantum *) NULL)
                break;
              for (x=0; x < (ssize_t) image->columns; x++)
              {
                *q++=(unsigned char) ClampToQuantum(GetPixelLuma(image,p));
                p+=GetPixelChannels(image);
              }
              if (image->previous == (Image *) NULL)
                {
                  status=SetImageProgress(image,SaveImageTag,(MagickOffsetType)
                    y,image->rows);
                  if (status == MagickFalse)
                    break;
                }
            }
          }
    (void) WriteBlob(image,(size_t) packets,pixels);
    pixel_info=RelinquishVirtualMemory(pixel_info);
    if (GetNextImageInList(image) == (Image *) NULL)
      break;
    image=SyncNextImageInList(image);
    status=SetImageProgress(image,SaveImagesTag,scene++,imageListLength);
    if (status == MagickFalse)
      break;
  } while (image_info->adjoin != MagickFalse);
  (void) CloseBlob(image);
  return(MagickTrue);
}