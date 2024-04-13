static MagickBooleanType GetOneAuthenticPixelFromStream(Image *image,
  const ssize_t x,const ssize_t y,Quantum *pixel,ExceptionInfo *exception)
{
  register Quantum
    *p;

  register ssize_t
    i;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  (void) memset(pixel,0,MaxPixelChannels*sizeof(*pixel));
  p=GetAuthenticPixelsStream(image,x,y,1,1,exception);
  if (p == (Quantum *) NULL)
    {
      pixel[RedPixelChannel]=ClampToQuantum(image->background_color.red);
      pixel[GreenPixelChannel]=ClampToQuantum(image->background_color.green);
      pixel[BluePixelChannel]=ClampToQuantum(image->background_color.blue);
      pixel[BlackPixelChannel]=ClampToQuantum(image->background_color.black);
      pixel[AlphaPixelChannel]=ClampToQuantum(image->background_color.alpha);
      return(MagickFalse);
    }
  for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
  {
    PixelChannel channel=GetPixelChannelChannel(image,i);
    pixel[channel]=p[i];
  }
  return(MagickTrue);
}