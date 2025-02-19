static MagickBooleanType WriteHDRImage(const ImageInfo *image_info,Image *image,
  ExceptionInfo *exception)
{
  char
    header[MagickPathExtent];

  const char
    *property;

  MagickBooleanType
    status;

  register const Quantum
    *p;

  register ssize_t
    i,
    x;

  size_t
    length;

  ssize_t
    count,
    y;

  unsigned char
    pixel[4],
    *pixels;

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
  if (IsRGBColorspace(image->colorspace) == MagickFalse)
    (void) TransformImageColorspace(image,RGBColorspace,exception);
  /*
    Write header.
  */
  (void) memset(header,' ',MagickPathExtent);
  length=CopyMagickString(header,"#?RADIANCE\n",MagickPathExtent);
  (void) WriteBlob(image,length,(unsigned char *) header);
  property=GetImageProperty(image,"comment",exception);
  if ((property != (const char *) NULL) &&
      (strchr(property,'\n') == (char *) NULL))
    {
      count=FormatLocaleString(header,MagickPathExtent,"#%.*s\n",
        MagickPathExtent-3,property);
      (void) WriteBlob(image,(size_t) count,(unsigned char *) header);
    }
  property=GetImageProperty(image,"hdr:exposure",exception);
  if (property != (const char *) NULL)
    {
      count=FormatLocaleString(header,MagickPathExtent,"EXPOSURE=%g\n",
        strtod(property,(char **) NULL));
      (void) WriteBlob(image,(size_t) count,(unsigned char *) header);
    }
  if (image->gamma != 0.0)
    {
      count=FormatLocaleString(header,MagickPathExtent,"GAMMA=%g\n",
        image->gamma);
      (void) WriteBlob(image,(size_t) count,(unsigned char *) header);
    }
  count=FormatLocaleString(header,MagickPathExtent,
    "PRIMARIES=%g %g %g %g %g %g %g %g\n",
    image->chromaticity.red_primary.x,image->chromaticity.red_primary.y,
    image->chromaticity.green_primary.x,image->chromaticity.green_primary.y,
    image->chromaticity.blue_primary.x,image->chromaticity.blue_primary.y,
    image->chromaticity.white_point.x,image->chromaticity.white_point.y);
  (void) WriteBlob(image,(size_t) count,(unsigned char *) header);
  length=CopyMagickString(header,"FORMAT=32-bit_rle_rgbe\n\n",MagickPathExtent);
  (void) WriteBlob(image,length,(unsigned char *) header);
  count=FormatLocaleString(header,MagickPathExtent,"-Y %.20g +X %.20g\n",
    (double) image->rows,(double) image->columns);
  (void) WriteBlob(image,(size_t) count,(unsigned char *) header);
  /*
    Write HDR pixels.
  */
  pixels=(unsigned char *) AcquireQuantumMemory(image->columns+128,4*
    sizeof(*pixels));
  if (pixels == (unsigned char *) NULL)
    ThrowWriterException(ResourceLimitError,"MemoryAllocationFailed");
  (void) memset(pixels,0,4*(image->columns+128)*sizeof(*pixels));
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    p=GetVirtualPixels(image,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    if ((image->columns >= 8) && (image->columns <= 0x7ffff))
      {
        pixel[0]=2;
        pixel[1]=2;
        pixel[2]=(unsigned char) (image->columns >> 8);
        pixel[3]=(unsigned char) (image->columns & 0xff);
        count=WriteBlob(image,4*sizeof(*pixel),pixel);
        if (count != (ssize_t) (4*sizeof(*pixel)))
          break;
      }
    i=0;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      double
        gamma;

      pixel[0]=0;
      pixel[1]=0;
      pixel[2]=0;
      pixel[3]=0;
      gamma=QuantumScale*GetPixelRed(image,p);
      if ((QuantumScale*GetPixelGreen(image,p)) > gamma)
        gamma=QuantumScale*GetPixelGreen(image,p);
      if ((QuantumScale*GetPixelBlue(image,p)) > gamma)
        gamma=QuantumScale*GetPixelBlue(image,p);
      if (gamma > MagickEpsilon)
        {
          int
            exponent;

          gamma=frexp(gamma,&exponent)*256.0/gamma;
          if (GetPixelRed(image,p) > 0)
            pixel[0]=(unsigned char) (gamma*QuantumScale*GetPixelRed(image,p));
          if (GetPixelGreen(image,p) > 0)
            pixel[1]=(unsigned char) (gamma*QuantumScale*
              GetPixelGreen(image,p));
          if (GetPixelBlue(image,p) > 0)
            pixel[2]=(unsigned char) (gamma*QuantumScale*GetPixelBlue(image,p));
          pixel[3]=(unsigned char) (exponent+128);
        }
      if ((image->columns >= 8) && (image->columns <= 0x7ffff))
        {
          pixels[x]=pixel[0];
          pixels[x+image->columns]=pixel[1];
          pixels[x+2*image->columns]=pixel[2];
          pixels[x+3*image->columns]=pixel[3];
        }
      else
        {
          pixels[i++]=pixel[0];
          pixels[i++]=pixel[1];
          pixels[i++]=pixel[2];
          pixels[i++]=pixel[3];
        }
      p+=GetPixelChannels(image);
    }
    if ((image->columns >= 8) && (image->columns <= 0x7ffff))
      {
        for (i=0; i < 4; i++)
          length=HDRWriteRunlengthPixels(image,&pixels[i*image->columns]);
      }
    else
      {
        count=WriteBlob(image,4*image->columns*sizeof(*pixels),pixels);
        if (count != (ssize_t) (4*image->columns*sizeof(*pixels)))
          break;
      }
    status=SetImageProgress(image,SaveImageTag,(MagickOffsetType) y,
      image->rows);
    if (status == MagickFalse)
      break;
  }
  pixels=(unsigned char *) RelinquishMagickMemory(pixels);
  (void) CloseBlob(image);
  return(MagickTrue);
}