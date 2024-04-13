MagickExport char *GetNextImageRegistry(void)
{
  if (IsEventLogging() != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"...");
  if (registry == (void *) NULL)
    return((char *) NULL);
  return((char *) GetNextKeyInSplayTree(registry));
}