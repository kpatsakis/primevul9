lock_seed_file (int fd, const char *fname, int for_write)
{
#ifdef __GCC__
#warning Check whether we can lock on Windows.
#endif
#if LOCK_SEED_FILE
  struct flock lck;
  struct timeval tv;
  int backoff=0;

  /* We take a lock on the entire file. */
  memset (&lck, 0, sizeof lck);
  lck.l_type = for_write? F_WRLCK : F_RDLCK;
  lck.l_whence = SEEK_SET;

  while (fcntl (fd, F_SETLK, &lck) == -1)
    {
      if (errno != EAGAIN && errno != EACCES)
        {
          log_info (_("can't lock `%s': %s\n"), fname, strerror (errno));
          return -1;
        }

      if (backoff > 2) /* Show the first message after ~2.25 seconds. */
        log_info( _("waiting for lock on `%s'...\n"), fname);

      tv.tv_sec = backoff;
      tv.tv_usec = 250000;
      select (0, NULL, NULL, NULL, &tv);
      if (backoff < 10)
        backoff++ ;
    }
#endif /*!LOCK_SEED_FILE*/
  return 0;
}