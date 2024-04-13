MagickExport PixelInfo *ClonePixelInfo(const PixelInfo *pixel)
{
  PixelInfo
    *pixel_info;

  pixel_info=(PixelInfo *) AcquireQuantumMemory(1,sizeof(*pixel_info));
  if (pixel_info == (PixelInfo *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  *pixel_info=(*pixel);
  return(pixel_info);
}