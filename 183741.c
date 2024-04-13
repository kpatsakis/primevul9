MagickExport void *RemoveImageRegistry(const char *key)
{
  if (IsEventLogging() != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",key);
  if (registry == (void *) NULL)
    return((void *) NULL);
  return(RemoveNodeFromSplayTree(registry,key));
}