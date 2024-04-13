static MagickBooleanType RenderX11(Image *image,const DrawInfo *draw_info,
  const PointInfo *offset,TypeMetric *metrics)
{
  MagickBooleanType
    status;

  if (annotate_semaphore == (SemaphoreInfo *) NULL)
    ActivateSemaphoreInfo(&annotate_semaphore);
  LockSemaphoreInfo(annotate_semaphore);
  status=XRenderImage(image,draw_info,offset,metrics);
  UnlockSemaphoreInfo(annotate_semaphore);
  return(status);
}