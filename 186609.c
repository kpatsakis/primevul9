act_on_fds_3_and_up (void (*func) (int fd))
{
  int maxfds, i;

#ifdef __linux__
  DIR *d;

  /* On Linux we can optimize this a bit if /proc is available. If it
     isn't available, fall back to the brute force way. */

  d = opendir ("/proc/self/fd");
  if (d)
    {
      for (;;)
        {
          struct dirent *de;
          int fd;
          long l;
          char *e = NULL;

          de = readdir (d);
          if (!de)
            break;

          if (de->d_name[0] == '.')
            continue;

          errno = 0;
          l = strtol (de->d_name, &e, 10);
          if (errno != 0 || e == NULL || *e != '\0')
            continue;

          fd = (int) l;
          if (fd < 3)
            continue;

          if (fd == dirfd (d))
            continue;

          func (fd);
        }

      closedir (d);
      return;
    }
#endif

  maxfds = sysconf (_SC_OPEN_MAX);

  /* Pick something reasonable if for some reason sysconf says
   * unlimited.
   */
  if (maxfds < 0)
    maxfds = 1024;

  /* close all inherited fds */
  for (i = 3; i < maxfds; i++)
    func (i);
}