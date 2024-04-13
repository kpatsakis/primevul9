void imap_make_date (char *buf, time_t timestamp)
{
  struct tm* tm = localtime (&timestamp);
  time_t tz = mutt_local_tz (timestamp);

  tz /= 60;

  snprintf (buf, IMAP_DATELEN, "%02d-%s-%d %02d:%02d:%02d %+03d%02d",
            tm->tm_mday, Months[tm->tm_mon], tm->tm_year + 1900,
            tm->tm_hour, tm->tm_min, tm->tm_sec,
            (int) tz / 60, (int) abs ((int) tz) % 60);
}