static MagickBooleanType GetOneVirtualPixelFromStream(const Image *image,
  const VirtualPixelMethod virtual_pixel_method,const ssize_t x,const ssize_t y,
  Quantum *pixel,ExceptionInfo *exception)
{
  const Quantum
    *p;

  register ssize_t
    i;

  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  (void) memset(pixel,0,MaxPixelChannels*sizeof(*pixel));
  p=GetVirtualPixelStream(image,virtual_pixel_method,x,y,1,1,exception);
  if (p == (const Quantum *) NULL)
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