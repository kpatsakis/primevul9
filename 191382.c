MagickExport MagickBooleanType SignatureImage(Image *image,
  ExceptionInfo *exception)
{
  CacheView
    *image_view;

  char
    *hex_signature;

  float
    pixel;

  const Quantum
    *p;

  SignatureInfo
    *signature_info;

  ssize_t
    y;

  StringInfo
    *signature;

  unsigned char
    *pixels;

  /*
    Compute image digital signature.
  */
  assert(image != (Image *) NULL);
  assert(image->signature == MagickCoreSignature);
  if (image->debug != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",image->filename);
  signature_info=AcquireSignatureInfo();
  signature=AcquireStringInfo(GetPixelChannels(image)*image->columns*
    sizeof(pixel));
  image_view=AcquireVirtualCacheView(image,exception);
  for (y=0; y < (ssize_t) image->rows; y++)
  {
    ssize_t
      x;

    unsigned char
      *q;

    p=GetCacheViewVirtualPixels(image_view,0,y,image->columns,1,exception);
    if (p == (const Quantum *) NULL)
      break;
    SetStringInfoLength(signature,GetPixelChannels(image)*image->columns*
      sizeof(pixel));
    pixels=GetStringInfoDatum(signature);
    q=pixels;
    for (x=0; x < (ssize_t) image->columns; x++)
    {
      ssize_t
        i;

      if (GetPixelReadMask(image,p) <= (QuantumRange/2))
        {
          p+=GetPixelChannels(image);
          continue;
        }
      for (i=0; i < (ssize_t) GetPixelChannels(image); i++)
      {
        ssize_t
          j;

        PixelChannel channel = GetPixelChannelChannel(image,i);
        PixelTrait traits = GetPixelChannelTraits(image,channel);
        if ((traits & UpdatePixelTrait) == 0)
          continue;
        pixel=(float) (QuantumScale*p[i]);
        if (signature_info->lsb_first == MagickFalse)
          for (j=(ssize_t) sizeof(pixel)-1; j >= 0; j--)
            *q++=(unsigned char) ((unsigned char *) &pixel)[j];
        else
          for (j=0; j < (ssize_t) sizeof(pixel); j++)
            *q++=(unsigned char) ((unsigned char *) &pixel)[j];
      }
      p+=GetPixelChannels(image);
    }
    SetStringInfoLength(signature,(size_t) (q-pixels));
    UpdateSignature(signature_info,signature);
  }
  image_view=DestroyCacheView(image_view);
  FinalizeSignature(signature_info);
  hex_signature=StringInfoToHexString(GetSignatureDigest(signature_info));
  (void) DeleteImageProperty(image,"signature");
  (void) SetImageProperty(image,"signature",hex_signature,exception);
  /*
    Free resources.
  */
  hex_signature=DestroyString(hex_signature);
  signature=DestroyStringInfo(signature);
  signature_info=DestroySignatureInfo(signature_info);
  return(MagickTrue);
}