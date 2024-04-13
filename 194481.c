MagickPrivate void LogComponentTerminus(void)
{
  if (event_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&event_semaphore);
  LockSemaphoreInfo(event_semaphore);
  UnlockSemaphoreInfo(event_semaphore);
  RelinquishSemaphoreInfo(&event_semaphore);
  if (log_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&log_semaphore);
  LockSemaphoreInfo(log_semaphore);
  if (log_cache != (LinkedListInfo *) NULL)
    log_cache=DestroyLinkedList(log_cache,DestroyLogElement);
  event_logging=MagickFalse;
  UnlockSemaphoreInfo(log_semaphore);
  RelinquishSemaphoreInfo(&log_semaphore);
}