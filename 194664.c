receive_statvfs(BOOL isspool, int *inodeptr)
{
#ifdef HAVE_STATFS
struct STATVFS statbuf;
struct stat dummy;
uschar *path;
uschar *name;
uschar buffer[1024];

/* The spool directory must always exist. */

if (isspool)
  {
  path = spool_directory;
  name = US"spool";
  }

/* Need to cut down the log file path to the directory, and to ignore any
appearance of "syslog" in it. */

else
  {
  int sep = ':';              /* Not variable - outside scripts use */
  const uschar *p = log_file_path;
  name = US"log";

  /* An empty log_file_path means "use the default". This is the same as an
  empty item in a list. */

  if (*p == 0) p = US":";
  while ((path = string_nextinlist(&p, &sep, buffer, sizeof(buffer))))
    if (Ustrcmp(path, "syslog") != 0)
      break;

  if (path == NULL)  /* No log files */
    {
    *inodeptr = -1;
    return -1;
    }

  /* An empty string means use the default, which is in the spool directory.
  But don't just use the spool directory, as it is possible that the log
  subdirectory has been symbolically linked elsewhere. */

  if (path[0] == 0)
    {
    sprintf(CS buffer, CS"%s/log", CS spool_directory);
    path = buffer;
    }
  else
    {
    uschar *cp;
    if ((cp = Ustrrchr(path, '/')) != NULL) *cp = 0;
    }
  }

/* We now have the path; do the business */

memset(&statbuf, 0, sizeof(statbuf));

if (STATVFS(CS path, &statbuf) != 0)
  if (stat(CS path, &dummy) == -1 && errno == ENOENT)
    {				/* Can happen on first run after installation */
    *inodeptr = -1;
    return -1;
    }
  else
    {
    log_write(0, LOG_MAIN|LOG_PANIC, "cannot accept message: failed to stat "
      "%s directory %s: %s", name, path, strerror(errno));
    smtp_closedown(US"spool or log directory problem");
    exim_exit(EXIT_FAILURE, NULL);
    }

*inodeptr = (statbuf.F_FILES > 0)? statbuf.F_FAVAIL : -1;

/* Disks are getting huge. Take care with computing the size in kilobytes. */

return (int)(((double)statbuf.F_BAVAIL * (double)statbuf.F_FRSIZE)/1024.0);

#else
/* Unable to find partition sizes in this environment. */

*inodeptr = -1;
return -1;
#endif
}