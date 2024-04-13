static void ExportGrayAlphaQuantum(const Image *image,QuantumInfo *quantum_info,
  const MagickSizeType number_pixels,const PixelPacket *magick_restrict p,
  unsigned char *magick_restrict q)
{
  QuantumAny
    range;

  ssize_t
    x;

  ssize_t
    bit;

  switch (quantum_info->depth)
  {
    case 1:
    {
      MagickRealType
        threshold;

      unsigned char
        black,
        pixel,
        white;

      black=0x00;
      white=0x01;
      if (quantum_info->min_is_white != MagickFalse)
        {
          black=0x01;
          white=0x00;
        }
      threshold=QuantumRange/2.0;
      for (x=((ssize_t) number_pixels-3); x > 0; x-=4)
      {
        *q='\0';
        *q|=(GetPixelLuma(image,p) > threshold ? black : white) << 7;
        pixel=(unsigned char) (GetPixelOpacity(p) == OpaqueOpacity ?
          0x00 : 0x01);
        *q|=(((int) pixel != 0 ? 0x00 : 0x01) << 6);
        p++;
        *q|=(GetPixelLuma(image,p) > threshold ? black : white) << 5;
        pixel=(unsigned char) (GetPixelOpacity(p) == OpaqueOpacity ?
          0x00 : 0x01);
        *q|=(((int) pixel != 0 ? 0x00 : 0x01) << 4);
        p++;
        *q|=(GetPixelLuma(image,p) > threshold ? black : white) << 3;
        pixel=(unsigned char) (GetPixelOpacity(p) == OpaqueOpacity ?
          0x00 : 0x01);
        *q|=(((int) pixel != 0 ? 0x00 : 0x01) << 2);
        p++;
        *q|=(GetPixelLuma(image,p) > threshold ? black : white) << 1;
        pixel=(unsigned char) (GetPixelOpacity(p) == OpaqueOpacity ?
          0x00 : 0x01);
        *q|=(((int) pixel != 0 ? 0x00 : 0x01) << 0);
        p++;
        q++;
      }
      if ((number_pixels % 4) != 0)
        {
          *q='\0';
          for (bit=0; bit <= (ssize_t) (number_pixels % 4); bit+=2)
          {
            *q|=(GetPixelLuma(image,p) > threshold ? black : white) <<
              (7-bit);
            pixel=(unsigned char) (GetPixelOpacity(p) == OpaqueOpacity ? 0x00 :
              0x01);
            *q|=(((int) pixel != 0 ? 0x00 : 0x01) << (unsigned char) (7-bit-1));
            p++;
          }
          q++;
        }
      break;
    }
    case 4:
    {
      unsigned char
        pixel;

      for (x=0; x < (ssize_t) number_pixels ; x++)
      {
        pixel=ScaleQuantumToChar(ClampToQuantum(GetPixelLuma(image,p)));
        *q=(((pixel >> 4) & 0xf) << 4);
        pixel=(unsigned char) (16*QuantumScale*((Quantum) (QuantumRange-
          GetPixelOpacity(p)))+0.5);
        *q|=pixel & 0xf;
        p++;
        q++;
      }
      break;
    }
    case 8:
    {
      unsigned char
        pixel;

      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToChar(ClampToQuantum(GetPixelLuma(image,p)));
        q=PopCharPixel(pixel,q);
        pixel=ScaleQuantumToChar((Quantum) (QuantumRange-GetPixelOpacity(p)));
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
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelLuma(image,p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelAlpha(p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToShort(ClampToQuantum(GetPixelLuma(image,p)));
        q=PopShortPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToShort((Quantum) (QuantumRange-GetPixelOpacity(p)));
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
            float
              pixel;

            pixel=(float) GetPixelLuma(image,p);
            q=PopFloatPixel(quantum_info,pixel,q);
            pixel=(float) (GetPixelAlpha(p));
            q=PopFloatPixel(quantum_info,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(ClampToQuantum(GetPixelLuma(image,p)));
        q=PopLongPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToLong((Quantum) (QuantumRange-GetPixelOpacity(p)));
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
            double
              pixel;

            pixel=(double) GetPixelLuma(image,p);
            q=PopDoublePixel(quantum_info,pixel,q);
            pixel=(double) (GetPixelAlpha(p));
            q=PopDoublePixel(quantum_info,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
    }
    default:
    {
      range=GetQuantumRange(quantum_info->depth);
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopQuantumPixel(quantum_info,
          ScaleQuantumToAny(ClampToQuantum(GetPixelLuma(image,p)),range),q);
        q=PopQuantumPixel(quantum_info,
          ScaleQuantumToAny((Quantum) (GetPixelAlpha(p)),range),q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
  }
}