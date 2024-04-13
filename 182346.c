static void ExportGrayQuantum(const Image *image,QuantumInfo *quantum_info,
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
        white;

      black=0x00;
      white=0x01;
      if (quantum_info->min_is_white != MagickFalse)
        {
          black=0x01;
          white=0x00;
        }
      threshold=QuantumRange/2.0;
      for (x=((ssize_t) number_pixels-7); x > 0; x-=8)
      {
        *q='\0';
        *q|=(GetPixelLuma(image,p) < threshold ? black : white) << 7;
        p++;
        *q|=(GetPixelLuma(image,p) < threshold ? black : white) << 6;
        p++;
        *q|=(GetPixelLuma(image,p) < threshold ? black : white) << 5;
        p++;
        *q|=(GetPixelLuma(image,p) < threshold ? black : white) << 4;
        p++;
        *q|=(GetPixelLuma(image,p) < threshold ? black : white) << 3;
        p++;
        *q|=(GetPixelLuma(image,p) < threshold ? black : white) << 2;
        p++;
        *q|=(GetPixelLuma(image,p) < threshold ? black : white) << 1;
        p++;
        *q|=(GetPixelLuma(image,p) < threshold ? black : white) << 0;
        p++;
        q++;
      }
      if ((number_pixels % 8) != 0)
        {
          *q='\0';
          for (bit=7; bit >= (ssize_t) (8-(number_pixels % 8)); bit--)
          {
            *q|=(GetPixelLuma(image,p) < threshold ? black : white) << bit;
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

      for (x=0; x < (ssize_t) (number_pixels-1) ; x+=2)
      {
        pixel=ScaleQuantumToChar(ClampToQuantum(GetPixelLuma(image,p)));
        *q=(((pixel >> 4) & 0xf) << 4);
        p++;
        pixel=ScaleQuantumToChar(ClampToQuantum(GetPixelLuma(image,p)));
        *q|=pixel >> 4;
        p++;
        q++;
      }
      if ((number_pixels % 2) != 0)
        {
          pixel=ScaleQuantumToChar(ClampToQuantum(GetPixelLuma(image,p)));
          *q=(((pixel >> 4) & 0xf) << 4);
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
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
    case 10:
    {
      range=GetQuantumRange(quantum_info->depth);
      if (quantum_info->pack == MagickFalse)
        {
          unsigned int
            pixel;

          for (x=0; x < (ssize_t) (number_pixels-2); x+=3)
          {
            pixel=(unsigned int) (
              ScaleQuantumToAny(ClampToQuantum(GetPixelLuma(image,p+2)),range) << 22 |
              ScaleQuantumToAny(ClampToQuantum(GetPixelLuma(image,p+1)),range) << 12 |
              ScaleQuantumToAny(ClampToQuantum(GetPixelLuma(image,p+0)),range) << 2);
            q=PopLongPixel(quantum_info->endian,pixel,q);
            p+=3;
            q+=quantum_info->pad;
          }
          if (x < (ssize_t) number_pixels)
            {
              pixel=0U;
              if (x++ < (ssize_t) (number_pixels-1))
                pixel|=ScaleQuantumToAny(ClampToQuantum(GetPixelLuma(image,
                  p+1)),range) << 12;
              if (x++ < (ssize_t) number_pixels)
                pixel|=ScaleQuantumToAny(ClampToQuantum(GetPixelLuma(image,
                  p+0)),range) << 2;
              q=PopLongPixel(quantum_info->endian,pixel,q);
            }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(ClampToQuantum(
          GetPixelLuma(image,p)),range),q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
    case 12:
    {
      unsigned short
        pixel;

      range=GetQuantumRange(quantum_info->depth);
      if (quantum_info->pack == MagickFalse)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            pixel=ScaleQuantumToShort(ClampToQuantum(GetPixelLuma(image,p)));
            q=PopShortPixel(quantum_info->endian,(unsigned short) (pixel >> 4),q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(ClampToQuantum(
          GetPixelLuma(image,p)),range),q);
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
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToShort(ClampToQuantum(GetPixelLuma(image,p)));
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
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(ClampToQuantum(GetPixelLuma(image,p)));
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
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
  }
}