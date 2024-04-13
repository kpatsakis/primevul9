MagickExport void GetQuantizeInfo(QuantizeInfo *quantize_info)
{
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  assert(quantize_info != (QuantizeInfo *) NULL);
  (void) memset(quantize_info,0,sizeof(*quantize_info));
  quantize_info->number_colors=256;
  quantize_info->dither=MagickTrue;
  quantize_info->dither_method=RiemersmaDitherMethod;
  quantize_info->colorspace=UndefinedColorspace;
  quantize_info->measure_error=MagickFalse;
  quantize_info->signature=MagickCoreSignature;
}