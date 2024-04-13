bool ispidowner(pid_t pid)
  {
  char        path[MAXPATHLEN];
  struct stat sbuf;

  /* build path to pid */
  snprintf(path, sizeof(path), "/proc/%d", pid);

  /* do the stat */
  /*   if it fails, assume not owner */
  if (stat(path, &sbuf) != 0)
    return(FALSE);
 
  /* see if caller is the owner of pid */
  if (getuid() != sbuf.st_uid)
    return(FALSE);

  /* caller is owner */
  return(TRUE);
  }