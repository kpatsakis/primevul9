MagickExport MagickRealType GetPixelIntensity(
  const Image *magick_restrict image,const Quantum *magick_restrict pixel)
{
  MagickRealType
    blue,
    green,
    red,
    intensity;

  red=(MagickRealType) GetPixelRed(image,pixel);
  green=(MagickRealType) GetPixelGreen(image,pixel);
  blue=(MagickRealType) GetPixelBlue(image,pixel);
  switch (image->intensity)
  {
    case AveragePixelIntensityMethod:
    {
      intensity=(red+green+blue)/3.0;
      break;
    }
    case BrightnessPixelIntensityMethod:
    {
      intensity=MagickMax(MagickMax(red,green),blue);
      break;
    }
    case LightnessPixelIntensityMethod:
    {
      intensity=(MagickMin(MagickMin(red,green),blue)+
        MagickMax(MagickMax(red,green),blue))/2.0;
      break;
    }
    case MSPixelIntensityMethod:
    {
      intensity=(MagickRealType) (((double) red*red+green*green+blue*blue)/
        (3.0*QuantumRange));
      break;
    }
    case Rec601LumaPixelIntensityMethod:
    {
      if ((image->colorspace == RGBColorspace) ||
          (image->colorspace == LinearGRAYColorspace))
        {
          red=EncodePixelGamma(red);
          green=EncodePixelGamma(green);
          blue=EncodePixelGamma(blue);
        }
      intensity=0.298839*red+0.586811*green+0.114350*blue;
      break;
    }
    case Rec601LuminancePixelIntensityMethod:
    {
      if ((image->colorspace == sRGBColorspace) ||
          (image->colorspace == GRAYColorspace))
        {
          red=DecodePixelGamma(red);
          green=DecodePixelGamma(green);
          blue=DecodePixelGamma(blue);
        }
      intensity=0.298839*red+0.586811*green+0.114350*blue;
      break;
    }
    case Rec709LumaPixelIntensityMethod:
    default:
    {
      if ((image->colorspace == RGBColorspace) ||
          (image->colorspace == LinearGRAYColorspace))
        {
          red=EncodePixelGamma(red);
          green=EncodePixelGamma(green);
          blue=EncodePixelGamma(blue);
        }
      intensity=0.212656*red+0.715158*green+0.072186*blue;
      break;
    }
    case Rec709LuminancePixelIntensityMethod:
    {
      if ((image->colorspace == sRGBColorspace) ||
          (image->colorspace == GRAYColorspace))
        {
          red=DecodePixelGamma(red);
          green=DecodePixelGamma(green);
          blue=DecodePixelGamma(blue);
        }
      intensity=0.212656*red+0.715158*green+0.072186*blue;
      break;
    }
    case RMSPixelIntensityMethod:
    {
      intensity=(MagickRealType) (sqrt((double) red*red+green*green+blue*blue)/
        sqrt(3.0));
      break;
    }
  }
  return(intensity);
}