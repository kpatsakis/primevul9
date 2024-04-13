open_temp_exec_file_dir (const char *dir)
{
  static const char suffix[] = "/ffiXXXXXX";
  int lendir, flags;
  char *tempname;
#ifdef O_TMPFILE
  int fd;
#endif

#ifdef O_CLOEXEC
  flags = O_CLOEXEC;
#else
  flags = 0;
#endif

#ifdef O_TMPFILE
  fd = open (dir, flags | O_RDWR | O_EXCL | O_TMPFILE, 0700);
  /* If the running system does not support the O_TMPFILE flag then retry without it. */
  if (fd != -1 || (errno != EINVAL && errno != EISDIR && errno != EOPNOTSUPP)) {
    return fd;
  } else {
    errno = 0;
  }
#endif

  lendir = (int) strlen (dir);
  tempname = __builtin_alloca (lendir + sizeof (suffix));

  if (!tempname)
    return -1;

  memcpy (tempname, dir, lendir);
  memcpy (tempname + lendir, suffix, sizeof (suffix));

  return open_temp_exec_file_name (tempname, flags);
}