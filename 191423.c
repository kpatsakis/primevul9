MagickPrivate unsigned int GetSignatureDigestsize(
  const SignatureInfo *signature_info)
{
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(signature_info != (SignatureInfo *) NULL);
  assert(signature_info->signature == MagickCoreSignature);
  return(signature_info->digestsize);
}