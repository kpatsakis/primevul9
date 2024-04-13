static AESInfo *DestroyAESInfo(AESInfo *aes_info)
{
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(aes_info != (AESInfo *) NULL);
  assert(aes_info->signature == MagickCoreSignature);
  if (aes_info->decipher_key != (unsigned int *) NULL)
    aes_info->decipher_key=(unsigned int *) RelinquishMagickMemory(
      aes_info->decipher_key);
  if (aes_info->encipher_key != (unsigned int *) NULL)
    aes_info->encipher_key=(unsigned int *) RelinquishMagickMemory(
      aes_info->encipher_key);
  if (aes_info->key != (StringInfo *) NULL)
    aes_info->key=DestroyStringInfo(aes_info->key);
  aes_info->signature=(~MagickCoreSignature);
  aes_info=(AESInfo *) RelinquishMagickMemory(aes_info);
  return(aes_info);
}