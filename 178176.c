read_seed_file (void)
{
  int fd;
  struct stat sb;
  unsigned char buffer[POOLSIZE];
  int n;

  gcry_assert (pool_is_locked);

  if (!seed_file_name)
    return 0;

#ifdef HAVE_DOSISH_SYSTEM
  fd = open( seed_file_name, O_RDONLY | O_BINARY );
#else
  fd = open( seed_file_name, O_RDONLY );
#endif
  if( fd == -1 && errno == ENOENT)
    {
      allow_seed_file_update = 1;
      return 0;
    }

  if (fd == -1 )
    {
      log_info(_("can't open `%s': %s\n"), seed_file_name, strerror(errno) );
      return 0;
    }
  if (lock_seed_file (fd, seed_file_name, 0))
    {
      close (fd);
      return 0;
    }
  if (fstat( fd, &sb ) )
    {
      log_info(_("can't stat `%s': %s\n"), seed_file_name, strerror(errno) );
      close(fd);
      return 0;
    }
  if (!S_ISREG(sb.st_mode) )
    {
      log_info(_("`%s' is not a regular file - ignored\n"), seed_file_name );
      close(fd);
      return 0;
    }
  if (!sb.st_size )
    {
      log_info(_("note: random_seed file is empty\n") );
      close(fd);
      allow_seed_file_update = 1;
      return 0;
    }
  if (sb.st_size != POOLSIZE )
    {
      log_info(_("warning: invalid size of random_seed file - not used\n") );
      close(fd);
      return 0;
    }

  do
    {
      n = read( fd, buffer, POOLSIZE );
    }
  while (n == -1 && errno == EINTR );

  if (n != POOLSIZE)
    {
      log_fatal(_("can't read `%s': %s\n"), seed_file_name,strerror(errno) );
      close(fd);/*NOTREACHED*/
      return 0;
    }

  close(fd);

  add_randomness( buffer, POOLSIZE, RANDOM_ORIGIN_INIT );
  /* add some minor entropy to the pool now (this will also force a mixing) */
  {
    pid_t x = getpid();
    add_randomness( &x, sizeof(x), RANDOM_ORIGIN_INIT );
  }
  {
    time_t x = time(NULL);
    add_randomness( &x, sizeof(x), RANDOM_ORIGIN_INIT );
  }
  {
    clock_t x = clock();
    add_randomness( &x, sizeof(x), RANDOM_ORIGIN_INIT );
  }

  /* And read a few bytes from our entropy source.  By using a level
   * of 0 this will not block and might not return anything with some
   * entropy drivers, however the rndlinux driver will use
   * /dev/urandom and return some stuff - Do not read too much as we
   * want to be friendly to the scare system entropy resource. */
  read_random_source ( RANDOM_ORIGIN_INIT, 16, GCRY_WEAK_RANDOM );

  allow_seed_file_update = 1;
  return 1;
}