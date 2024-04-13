MagickPrivate SignatureInfo *AcquireSignatureInfo(void)
{
  SignatureInfo
    *signature_info;

  unsigned long
    lsb_first;

  signature_info=(SignatureInfo *) AcquireCriticalMemory(
    sizeof(*signature_info));
  (void) memset(signature_info,0,sizeof(*signature_info));
  signature_info->digestsize=SignatureDigestsize;
  signature_info->blocksize=SignatureBlocksize;
  signature_info->digest=AcquireStringInfo(SignatureDigestsize);
  signature_info->message=AcquireStringInfo(SignatureBlocksize);
  signature_info->accumulator=(unsigned int *) AcquireQuantumMemory(
    SignatureBlocksize,sizeof(*signature_info->accumulator));
  if (signature_info->accumulator == (unsigned int *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  (void) memset(signature_info->accumulator,0,SignatureBlocksize*
    sizeof(*signature_info->accumulator));
  lsb_first=1;
  signature_info->lsb_first=(int) (*(char *) &lsb_first) == 1 ? MagickTrue :
    MagickFalse;
  signature_info->timestamp=(ssize_t) GetMagickTime();
  signature_info->signature=MagickCoreSignature;
  InitializeSignature(signature_info);
  return(signature_info);
}