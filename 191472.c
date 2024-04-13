MagickPrivate SignatureInfo *DestroySignatureInfo(SignatureInfo *signature_info)
{
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(signature_info != (SignatureInfo *) NULL);
  assert(signature_info->signature == MagickCoreSignature);
  if (signature_info->accumulator != (unsigned int *) NULL)
    signature_info->accumulator=(unsigned int *) RelinquishMagickMemory(
      signature_info->accumulator);
  if (signature_info->message != (StringInfo *) NULL)
    signature_info->message=DestroyStringInfo(signature_info->message);
  if (signature_info->digest != (StringInfo *) NULL)
    signature_info->digest=DestroyStringInfo(signature_info->digest);
  signature_info->signature=(~MagickCoreSignature);
  signature_info=(SignatureInfo *) RelinquishMagickMemory(signature_info);
  return(signature_info);
}