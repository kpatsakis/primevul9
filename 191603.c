dns_lookup_timerwrap(dns_answer *dnsa, const uschar *name, int type,
  const uschar **fully_qualified_name)
{
int retval;
unsigned long time_msec;

if (!slow_lookup_log)
  return dns_lookup(dnsa, name, type, fully_qualified_name);

time_msec = get_time_in_ms();
retval = dns_lookup(dnsa, name, type, fully_qualified_name);
if ((time_msec = get_time_in_ms() - time_msec) > slow_lookup_log)
  log_long_lookup(dns_text_type(type), name, time_msec);
return retval;
}