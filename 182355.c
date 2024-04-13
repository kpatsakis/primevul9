static void ExportRGBOQuantum(QuantumInfo *quantum_info,
  const MagickSizeType number_pixels,const PixelPacket *magick_restrict p,
  unsigned char *magick_restrict q)
{
  QuantumAny
    range;

  ssize_t
    x;

  switch (quantum_info->depth)
  {
    case 8:
    {
      unsigned char
        pixel;

      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToChar(GetPixelRed(p));
        q=PopCharPixel(pixel,q);
        pixel=ScaleQuantumToChar(GetPixelGreen(p));
        q=PopCharPixel(pixel,q);
        pixel=ScaleQuantumToChar(GetPixelBlue(p));
        q=PopCharPixel(pixel,q);
        pixel=ScaleQuantumToChar(GetPixelOpacity(p));
        q=PopCharPixel(pixel,q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
    case 10:
    {
      unsigned int
        pixel;

      range=GetQuantumRange(quantum_info->depth);
      if (quantum_info->pack == MagickFalse)
        {
          ssize_t
            i;

          size_t
            quantum;

          ssize_t
            n;

          n=0;
          quantum=0;
          pixel=0;
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            for (i=0; i < 4; i++)
            {
              switch (i)
              {
                case 0: quantum=GetPixelRed(p); break;
                case 1: quantum=GetPixelGreen(p); break;
                case 2: quantum=GetPixelBlue(p); break;
                case 3: quantum=GetPixelOpacity(p); break;
              }
              switch (n % 3)
              {
                case 0:
                {
                  pixel|=(size_t) (ScaleQuantumToAny((Quantum) quantum,
                    range) << 22);
                  break;
                }
                case 1:
                {
                  pixel|=(size_t) (ScaleQuantumToAny((Quantum) quantum,
                    range) << 12);
                  break;
                }
                case 2:
                {
                  pixel|=(size_t) (ScaleQuantumToAny((Quantum) quantum,
                    range) << 2);
                  q=PopLongPixel(quantum_info->endian,pixel,q);
                  pixel=0;
                  break;
                }
              }
              n++;
            }
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      if (quantum_info->quantum == 32UL)
        {
          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelRed(p),range);
            q=PopQuantumLongPixel(quantum_info,pixel,q);
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelGreen(p),range);
            q=PopQuantumLongPixel(quantum_info,pixel,q);
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelBlue(p),range);
            q=PopQuantumLongPixel(quantum_info,pixel,q);
            pixel=(unsigned int) ScaleQuantumToAny(GetPixelOpacity(p),range);
            q=PopQuantumLongPixel(quantum_info,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelRed(p),range);
        q=PopQuantumPixel(quantum_info,pixel,q);
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelGreen(p),range);
        q=PopQuantumPixel(quantum_info,pixel,q);
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelBlue(p),range);
        q=PopQuantumPixel(quantum_info,pixel,q);
        pixel=(unsigned int) ScaleQuantumToAny(GetPixelOpacity(p),range);
        q=PopQuantumPixel(quantum_info,pixel,q);
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
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelRed(p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelGreen(p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelBlue(p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            pixel=SinglePrecisionToHalf(QuantumScale*GetPixelOpacity(p));
            q=PopShortPixel(quantum_info->endian,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToShort(GetPixelRed(p));
        q=PopShortPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToShort(GetPixelGreen(p));
        q=PopShortPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToShort(GetPixelBlue(p));
        q=PopShortPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToShort(GetPixelOpacity(p));
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

            q=PopFloatPixel(quantum_info,(float) GetPixelRed(p),q);
            q=PopFloatPixel(quantum_info,(float) GetPixelGreen(p),q);
            q=PopFloatPixel(quantum_info,(float) GetPixelBlue(p),q);
            pixel=(float) GetPixelOpacity(p);
            q=PopFloatPixel(quantum_info,pixel,q);
            p++;
            q+=quantum_info->pad;
          }
          break;
        }
      for (x=0; x < (ssize_t) number_pixels; x++)
      {
        pixel=ScaleQuantumToLong(GetPixelRed(p));
        q=PopLongPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToLong(GetPixelGreen(p));
        q=PopLongPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToLong(GetPixelBlue(p));
        q=PopLongPixel(quantum_info->endian,pixel,q);
        pixel=ScaleQuantumToLong(GetPixelOpacity(p));
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
          double
            pixel;

          for (x=0; x < (ssize_t) number_pixels; x++)
          {
            q=PopDoublePixel(quantum_info,(double) GetPixelRed(p),q);
            q=PopDoublePixel(quantum_info,(double) GetPixelGreen(p),q);
            q=PopDoublePixel(quantum_info,(double) GetPixelBlue(p),q);
            pixel=(double) GetPixelOpacity(p);
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
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(GetPixelRed(p),
          range),q);
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(GetPixelGreen(p),
          range),q);
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(GetPixelBlue(p),
          range),q);
        q=PopQuantumPixel(quantum_info,ScaleQuantumToAny(GetPixelOpacity(p),
          range),q);
        p++;
        q+=quantum_info->pad;
      }
      break;
    }
  }
}