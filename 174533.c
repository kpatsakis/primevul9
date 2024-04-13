string_timediff(struct timeval * diff)
{
static uschar buf[sizeof("0.000s")];

if (diff->tv_sec >= 5 || !LOGGING(millisec))
  return readconf_printtime((int)diff->tv_sec);

sprintf(CS buf, "%d.%03ds", (int)diff->tv_sec, (int)diff->tv_usec/1000);
return buf;
}