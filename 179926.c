MagickExport MagickRealType GetPixelInfoIntensity(
  const Image *magick_restrict image,const PixelInfo *magick_restrict pixel)
{
  MagickRealType
    blue,
    green,
    red,
    intensity;

  PixelIntensityMethod
    method;

  method=Rec709LumaPixelIntensityMethod;
  if (image != (const Image *) NULL)
    method=image->intensity;
  red=pixel->red;
  green=pixel->green;
  blue=pixel->blue;
  switch (method)
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
      if (pixel->colorspace == RGBColorspace)
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
      if (pixel->colorspace == sRGBColorspace)
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
      if (pixel->colorspace == RGBColorspace)
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
      if (pixel->colorspace == sRGBColorspace)
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