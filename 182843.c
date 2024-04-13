zone_debuglog(dns_zone_t *zone, const char *me, int debuglevel,
	      const char *fmt, ...)
{
	int level = ISC_LOG_DEBUG(debuglevel);
	va_list ap;

	va_start(ap, fmt);
	dns_zone_logv(zone, DNS_LOGCATEGORY_GENERAL, level, me, fmt, ap);
	va_end(ap);
}