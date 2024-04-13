static MagickBooleanType ReadVIPSPixelsNONE(Image *image,
  const VIPSBandFormat format,const VIPSType type,const unsigned int channels,
  ExceptionInfo *exception)
{
  Quantum
    pixel;

  register Quantum
    *q;

  register ssize_t
    x;

  ssize_t
    y;

  for (y = 0; y < (ssize_t) image->rows; y++)
  {
    q=GetAuthenticPixels(image,0,y,image->columns,1,exception);
    if (q == (Quantum *) NULL)
      return MagickFalse;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      pixel=ReadVIPSPixelNONE(image,format,type);
      SetPixelRed(image,pixel,q);
      if (channels < 3)
        {
          SetPixelGreen(image,pixel,q);
          SetPixelBlue(image,pixel,q);
          if (channels == 2)
            SetPixelAlpha(image,ReadVIPSPixelNONE(image,format,type),q);
        }
      else
        {
          SetPixelGreen(image,ReadVIPSPixelNONE(image,format,type),q);
          SetPixelBlue(image,ReadVIPSPixelNONE(image,format,type),q);
          if (channels == 4)
            {
              if (image->colorspace == CMYKColorspace)
                SetPixelIndex(image,ReadVIPSPixelNONE(image,format,type),q);
              else
                SetPixelAlpha(image,ReadVIPSPixelNONE(image,format,type),q);
            }
          else if (channels == 5)
            {
              SetPixelIndex(image,ReadVIPSPixelNONE(image,format,type),q);
              SetPixelAlpha(image,ReadVIPSPixelNONE(image,format,type),q);
            }
        }
      q+=GetPixelChannels(image);
    }
    if (SyncAuthenticPixels(image,exception) == MagickFalse)
      return MagickFalse;
  }
  return(MagickTrue);
}