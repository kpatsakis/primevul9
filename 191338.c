MagickExport void AnnotateComponentTerminus(void)
{
  if (annotate_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&annotate_semaphore);
  DestroySemaphoreInfo(&annotate_semaphore);
}