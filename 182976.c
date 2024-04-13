dns_zone_logc(dns_zone_t *zone, isc_logcategory_t *category,
	      int level, const char *fmt, ...)
{
	va_list ap;

	va_start(ap, fmt);
	dns_zone_logv(zone, category, level, NULL, fmt, ap);
	va_end(ap);
}