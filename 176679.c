static void polarssl_debug(void *context, int level, const char *line)
{
  struct SessionHandle *data = NULL;

  if(!context)
    return;

  data = (struct SessionHandle *)context;

  infof(data, "%s", line);
  (void) level;
}