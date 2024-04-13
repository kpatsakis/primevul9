static MagickBooleanType WriteVIPSImage(const ImageInfo *image_info,
  Image *image,ExceptionInfo *exception)
{
  const char
    *metadata;

  MagickBooleanType
    status;

  register const Quantum
    *p;

  register ssize_t
    x;

  ssize_t
    y;

  unsigned int
    channels;

  assert(image_info != (const ImageInfo *) NULL);
  assert(image_info->signature == MagickCoreSignature);
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);

  status=OpenBlob(image_info,image,WriteBinaryBlobMode,exception);
  if (status == MagickFalse)
    return(status);
  if (image->endian == LSBEndian)
    (void) WriteBlobLSBLong(image,VIPS_MAGIC_LSB);
  else
    (void) WriteBlobLSBLong(image,VIPS_MAGIC_MSB);
  (void) WriteBlobLong(image,(unsigned int) image->columns);
  (void) WriteBlobLong(image,(unsigned int) image->rows);
  (void) SetImageStorageClass(image,DirectClass,exception);
  channels=image->alpha_trait != UndefinedPixelTrait ? 4 : 3;
  if (SetImageGray(image,exception) != MagickFalse)
    channels=image->alpha_trait != UndefinedPixelTrait ? 2 : 1;
  else if (image->colorspace == CMYKColorspace)
    channels=image->alpha_trait != UndefinedPixelTrait ? 5 : 4;
  (void) WriteBlobLong(image,channels);
  (void) WriteBlobLong(image,0);
  if (image->depth == 16)
    (void) WriteBlobLong(image,(unsigned int) VIPSBandFormatUSHORT);
  else
    {
      image->depth=8;
      (void) WriteBlobLong(image,(unsigned int) VIPSBandFormatUCHAR);
    }
  (void) WriteBlobLong(image,VIPSCodingNONE);
  switch(image->colorspace)
  {
    case CMYKColorspace:
      (void) WriteBlobLong(image,VIPSTypeCMYK);
      break;
    case GRAYColorspace:
      if (image->depth == 16)
        (void) WriteBlobLong(image, VIPSTypeGREY16);
      else
        (void) WriteBlobLong(image, VIPSTypeB_W);
      break;
    case LabColorspace:
      (void) WriteBlobLong(image,VIPSTypeLAB);
      break;
    case LCHColorspace:
      (void) WriteBlobLong(image,VIPSTypeLCH);
      break;
    case RGBColorspace:
      if (image->depth == 16)
        (void) WriteBlobLong(image, VIPSTypeRGB16);
      else
        (void) WriteBlobLong(image, VIPSTypeRGB);
      break;
    case XYZColorspace:
      (void) WriteBlobLong(image,VIPSTypeXYZ);
      break;
    default:
    case sRGBColorspace:
      (void) SetImageColorspace(image,sRGBColorspace,exception);
      (void) WriteBlobLong(image,VIPSTypesRGB);
      break;
  }
  if (image->units == PixelsPerCentimeterResolution)
    {
      (void) WriteBlobFloat(image,(image->resolution.x / 10));
      (void) WriteBlobFloat(image,(image->resolution.y / 10));
    }
  else if (image->units == PixelsPerInchResolution)
    {
      (void) WriteBlobFloat(image,(image->resolution.x / 25.4));
      (void) WriteBlobFloat(image,(image->resolution.y / 25.4));
    }
  else
    {
      (void) WriteBlobLong(image,0);
      (void) WriteBlobLong(image,0);
    }
  /*
    Legacy, Offsets, Future
  */
  for (y=0; y < 24; y++)
    (void) WriteBlobByte(image,0);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      WriteVIPSPixel(image,GetPixelRed(image,p));
      if (channels == 2)
        WriteVIPSPixel(image,GetPixelAlpha(image,p));
      else
        {
          WriteVIPSPixel(image,GetPixelGreen(image,p));
          WriteVIPSPixel(image,GetPixelBlue(image,p));
          if (channels >= 4)
            {
              if (image->colorspace == CMYKColorspace)
                WriteVIPSPixel(image,GetPixelIndex(image,p));
              else
                WriteVIPSPixel(image,GetPixelAlpha(image,p));
            }
          else if (channels == 5)
            {
               WriteVIPSPixel(image,GetPixelIndex(image,p));
               WriteVIPSPixel(image,GetPixelAlpha(image,p));
            }
        }
      p+=GetPixelChannels(image);
    }
  }
  metadata=GetImageProperty(image,"vips:metadata",exception);
  if (metadata != (const char*) NULL)
    WriteBlobString(image,metadata);
  (void) CloseBlob(image);
  return(status);
}