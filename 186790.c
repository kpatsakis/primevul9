gather_time_entropy(void)
{
#ifdef WIN32
  FILETIME ft;
  GetSystemTimeAsFileTime(&ft); /* never fails */
  return ft.dwHighDateTime ^ ft.dwLowDateTime;
#else
  struct timeval tv;
  int gettimeofday_res;

  gettimeofday_res = gettimeofday(&tv, NULL);
  assert (gettimeofday_res == 0);

  /* Microseconds time is <20 bits entropy */
  return tv.tv_usec;
#endif
}