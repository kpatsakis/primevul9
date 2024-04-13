MagickPrivate void RegistryComponentTerminus(void)
{
  if (registry_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&registry_semaphore);
  LockSemaphoreInfo(registry_semaphore);
  if (IsEventLogging() != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  if (registry != (void *) NULL)
    registry=DestroySplayTree(registry);
  UnlockSemaphoreInfo(registry_semaphore);
  RelinquishSemaphoreInfo(&registry_semaphore);
}