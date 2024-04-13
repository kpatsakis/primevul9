static void *DestroyDelegate(void *delegate_info)
{
  register DelegateInfo
    *p;

  p=(DelegateInfo *) delegate_info;
  if (p->path != (char *) NULL)
    p->path=DestroyString(p->path);
  if (p->decode != (char *) NULL)
    p->decode=DestroyString(p->decode);
  if (p->encode != (char *) NULL)
    p->encode=DestroyString(p->encode);
  if (p->commands != (char *) NULL)
    p->commands=DestroyString(p->commands);
  if (p->semaphore != (SemaphoreInfo *) NULL)
    RelinquishSemaphoreInfo(&p->semaphore);
  p=(DelegateInfo *) RelinquishMagickMemory(p);
  return((void *) NULL);
}