MagickExport void ResetImageRegistryIterator(void)
{
  if (IsEventLogging() != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  if (registry == (void *) NULL)
    return;
  ResetSplayTreeIterator(registry);
}