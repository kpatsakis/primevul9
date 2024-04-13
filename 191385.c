MagickPrivate void UpdateSignature(SignatureInfo *signature_info,
  const StringInfo *message)
{
  size_t
    i;

  unsigned char
    *p;

  size_t
    n;

  unsigned int
    length;

  /*
    Update the Signature accumulator.
  */
  assert(signature_info != (SignatureInfo *) NULL);
  assert(signature_info->signature == MagickCoreSignature);
  n=GetStringInfoLength(message);
  length=Trunc32((unsigned int) (signature_info->low_order+(n << 3)));
  if (length < signature_info->low_order)
    signature_info->high_order++;
  signature_info->low_order=length;
  signature_info->high_order+=(unsigned int) n >> 29;
  p=GetStringInfoDatum(message);
  if (signature_info->extent != 0)
    {
      i=GetStringInfoLength(signature_info->message)-signature_info->extent;
      if (i > n)
        i=n;
      (void) memcpy(GetStringInfoDatum(signature_info->message)+
        signature_info->extent,p,i);
      n-=i;
      p+=i;
      signature_info->extent+=i;
      if (signature_info->extent != GetStringInfoLength(signature_info->message))
        return;
      TransformSignature(signature_info);
    }
  while (n >= GetStringInfoLength(signature_info->message))
  {
    SetStringInfoDatum(signature_info->message,p);
    p+=GetStringInfoLength(signature_info->message);
    n-=GetStringInfoLength(signature_info->message);
    TransformSignature(signature_info);
  }
  (void) memcpy(GetStringInfoDatum(signature_info->message),p,n);
  signature_info->extent=n;
}