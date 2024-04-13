open_temp_exec_file_name (char *name, int flags)
{
  int fd;

#ifdef HAVE_MKOSTEMP
  fd = mkostemp (name, flags);
#else
  fd = mkstemp (name);
#endif

  if (fd != -1)
    unlink (name);

  return fd;
}