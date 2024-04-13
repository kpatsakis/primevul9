MagickExport MagickBooleanType AnnotateComponentGenesis(void)
{
  if (annotate_semaphore == (SemaphoreInfo *) NULL)
    annotate_semaphore=AllocateSemaphoreInfo();
  return(MagickTrue);
}