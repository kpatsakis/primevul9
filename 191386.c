MagickPrivate void InitializeSignature(SignatureInfo *signature_info)
{
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(signature_info != (SignatureInfo *) NULL);
  assert(signature_info->signature == MagickCoreSignature);
  signature_info->accumulator[0]=0x6a09e667U;
  signature_info->accumulator[1]=0xbb67ae85U;
  signature_info->accumulator[2]=0x3c6ef372U;
  signature_info->accumulator[3]=0xa54ff53aU;
  signature_info->accumulator[4]=0x510e527fU;
  signature_info->accumulator[5]=0x9b05688cU;
  signature_info->accumulator[6]=0x1f83d9abU;
  signature_info->accumulator[7]=0x5be0cd19U;
  signature_info->low_order=0;
  signature_info->high_order=0;
  signature_info->extent=0;
}