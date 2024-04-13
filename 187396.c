MagickExport QuantizeInfo *CloneQuantizeInfo(const QuantizeInfo *quantize_info)
{
  QuantizeInfo
    *clone_info;

  clone_info=(QuantizeInfo *) AcquireMagickMemory(sizeof(*clone_info));
  if (clone_info == (QuantizeInfo *) NULL)
    ThrowFatalException(ResourceLimitFatalError,"MemoryAllocationFailed");
  GetQuantizeInfo(clone_info);
  if (quantize_info == (QuantizeInfo *) NULL)
    return(clone_info);
  clone_info->number_colors=quantize_info->number_colors;
  clone_info->tree_depth=quantize_info->tree_depth;
  clone_info->dither=quantize_info->dither;
  clone_info->dither_method=quantize_info->dither_method;
  clone_info->colorspace=quantize_info->colorspace;
  clone_info->measure_error=quantize_info->measure_error;
  return(clone_info);
}