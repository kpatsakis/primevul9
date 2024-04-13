MagickExport MagickBooleanType GetDelegateThreadSupport(
  const DelegateInfo *delegate_info)
{
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");

  assert(delegate_info != (DelegateInfo *) NULL);
  assert(delegate_info->signature == MagickCoreSignature);
  return(delegate_info->thread_support);
}