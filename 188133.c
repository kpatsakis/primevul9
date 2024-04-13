MagickPrivate MagickBooleanType DelegateComponentGenesis(void)
{
  if (delegate_semaphore == (SemaphoreInfo *) NULL)
    delegate_semaphore=AcquireSemaphoreInfo();
  return(MagickTrue);
}