static void ExportBlackQuantum(const Image *image,QuantumInfo *quantum_info,
  const MagickSizeType number_pixels,const PixelPacket *magick_restrict p,
  const IndexPacket *magick_restrict indexes,unsigned char *magick_restrict q,
  ExceptionInfo *exception)
{
  ssize_t
    x;

  if (image->colorspace != CMYKColorspace)
    {
      (void) ThrowMagickException(exception,GetMagickModule(),ImageError,
        "ColorSeparatedImageRequired","`%s'",image->filename);
      return;
    }
  switch (quantum_info->depth)
  {
    case 8:
    {
      unsigned char
        pixel;

      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToChar(GetPixelIndex(indexes+x));
        q=PopCharPixel(pixel,q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
    case 16:
    {
      unsigned short
        pixel;

      if (quantum_info->format == FloatingPointQuantumFormat)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelIndex(indexes+x));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToShort(GetPixelIndex(indexes+x));
        q=PopShortPixel(quantum_info->endian,pixel,q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
    case 32:
    {
      unsigned int
        pixel;

      if (quantum_info->format == FloatingPointQuantumFormat)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            q=PopFloatPixel(quantum_info,(float) GetPixelIndex(indexes+x),q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(GetPixelIndex(indexes+x));
        q=PopLongPixel(quantum_info->endian,pixel,q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
    case 64:
    {
      if (quantum_info->format == FloatingPointQuantumFormat)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            q=PopDoublePixel(quantum_info,(double) GetPixelIndex(indexes+x),
              q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
    }
    default:
    {
      QuantumAny
        range;

      range=GetQuantumRange(quantum_info->depth);
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopQuantumPixel(quantum_info,
          ScaleQuantumToAny((Quantum) GetPixelIndex(indexes+x),range),q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
  }
}