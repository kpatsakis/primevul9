void notef(struct GlobalConfig *config, const char *fmt, ...)
{
  va_list ap;
  va_start(ap, fmt);
  if(config->tracetype)
    voutf(config, NOTE_PREFIX, fmt, ap);
  va_end(ap);
}