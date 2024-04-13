MagickExport const char *GetDelegateCommands(const DelegateInfo *delegate_info)
{
  (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");

  assert(delegate_info != (DelegateInfo *) NULL);
  assert(delegate_info->signature == MagickCoreSignature);
  return(delegate_info->commands);
}