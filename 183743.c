MagickPrivate MagickBooleanType RegistryComponentGenesis(void)
{
  if (registry_semaphore == (SemaphoreInfo *) NULL)
    registry_semaphore=AcquireSemaphoreInfo();
  return(MagickTrue);
}