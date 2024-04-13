MagickExport MagickBooleanType LogMagickEvent(const LogEventType type,
  const char *module,const char *function,const size_t line,
  const char *format,...)
{
  va_list
    operands;

  MagickBooleanType
    status;

  if (IsEventLogging() == MagickFalse)
    return(MagickFalse);
  va_start(operands,format);
  status=LogMagickEventList(type,module,function,line,format,operands);
  va_end(operands);
  return(status);
}