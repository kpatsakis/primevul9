_dbus_get_monotonic_time (long *tv_sec,
                          long *tv_usec)
{
#ifdef HAVE_MONOTONIC_CLOCK
  struct timespec ts;
  clock_gettime (CLOCK_MONOTONIC, &ts);

  if (tv_sec)
    *tv_sec = ts.tv_sec;
  if (tv_usec)
    *tv_usec = ts.tv_nsec / 1000;
#else
  struct timeval t;

  gettimeofday (&t, NULL);

  if (tv_sec)
    *tv_sec = t.tv_sec;
  if (tv_usec)
    *tv_usec = t.tv_usec;
#endif
}