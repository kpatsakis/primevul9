MagickPrivate void DelegateComponentTerminus(void)
{
  if (delegate_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&delegate_semaphore);
  LockSemaphoreInfo(delegate_semaphore);
  if (delegate_cache != (LinkedListInfo *) NULL)
    delegate_cache=DestroyLinkedList(delegate_cache,DestroyDelegate);
  UnlockSemaphoreInfo(delegate_semaphore);
  RelinquishSemaphoreInfo(&delegate_semaphore);
}