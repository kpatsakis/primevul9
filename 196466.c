MagickPrivate void CacheComponentTerminus(void)
{
  if (cache_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&cache_semaphore);
  /* no op-- nothing to destroy */
  RelinquishSemaphoreInfo(&cache_semaphore);
}