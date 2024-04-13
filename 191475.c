MagickPrivate void FinalizeSignature(SignatureInfo *signature_info)
{
  ssize_t
    i;

  unsigned char
    *q;

  unsigned int
    *p;

  size_t
    extent;

  unsigned char
    *datum;

  unsigned int
    high_order,
    low_order;

  /*
    Add padding and return the message accumulator.
  */
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(signature_info != (SignatureInfo *) NULL);
  assert(signature_info->signature == MagickCoreSignature);
  low_order=signature_info->low_order;
  high_order=signature_info->high_order;
  extent=((low_order >> 3) & 0x3f);
  datum=GetStringInfoDatum(signature_info->message);
  datum[extent++]=(unsigned char) 0x80;
  if (extent <= (unsigned int) (GetStringInfoLength(signature_info->message)-8))
    (void) memset(datum+extent,0,GetStringInfoLength(
      signature_info->message)-8-extent);
  else
    {
      (void) memset(datum+extent,0,GetStringInfoLength(
        signature_info->message)-extent);
      TransformSignature(signature_info);
      (void) memset(datum,0,GetStringInfoLength(
        signature_info->message)-8);
    }
  datum[56]=(unsigned char) (high_order >> 24);
  datum[57]=(unsigned char) (high_order >> 16);
  datum[58]=(unsigned char) (high_order >> 8);
  datum[59]=(unsigned char) high_order;
  datum[60]=(unsigned char) (low_order >> 24);
  datum[61]=(unsigned char) (low_order >> 16);
  datum[62]=(unsigned char) (low_order >> 8);
  datum[63]=(unsigned char) low_order;
  TransformSignature(signature_info);
  p=signature_info->accumulator;
  q=GetStringInfoDatum(signature_info->digest);
  for (i=0; i < (SignatureDigestsize/4); i++)
  {
    *q++=(unsigned char) ((*p >> 24) & 0xff);
    *q++=(unsigned char) ((*p >> 16) & 0xff);
    *q++=(unsigned char) ((*p >> 8) & 0xff);
    *q++=(unsigned char) (*p & 0xff);
    p++;
  }
}