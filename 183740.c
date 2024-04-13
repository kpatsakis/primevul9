MagickExport MagickBooleanType DeleteImageRegistry(const char *key)
{
  if (IsEventLogging() != MagickFalse)
    (void) LogMagickEvent(TraceEvent,GetMagickModule(),"%s",key);
  if (registry == (void *) NULL)
    return(MagickFalse);
  return(DeleteNodeFromSplayTree(registry,key));
}