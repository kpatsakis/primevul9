static inline MagickBooleanType IsPathWritable(const char *path)
{
  if (IsPathAccessible(path) == MagickFalse)
    return(MagickFalse);
  if (access_utf8(path,W_OK) != 0)
    return(MagickFalse);
  return(MagickTrue);
}