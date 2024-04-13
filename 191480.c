static AESInfo *AcquireAESInfo(void)
{
  AESInfo
    *aes_info;

  aes_info=(AESInfo *) AcquireCriticalMemory(sizeof(*aes_info));
  (void) memset(aes_info,0,sizeof(*aes_info));
  aes_info->blocksize=AESBlocksize;
  aes_info->key=AcquireStringInfo(32);
  aes_info->encipher_key=(unsigned int *) AcquireQuantumMemory(60UL,sizeof(
    *aes_info->encipher_key));
  aes_info->decipher_key=(unsigned int *) AcquireQuantumMemory(60UL,sizeof(
    *aes_info->decipher_key));
  if ((aes_info->key == (StringInfo *) NULL) ||
      (aes_info->encipher_key == (unsigned int *) NULL) ||
      (aes_info->decipher_key == (unsigned int *) NULL))
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  aes_info->timestamp=(ssize_t) time(0);
  aes_info->signature=MagickCoreSignature;
  return(aes_info);
}