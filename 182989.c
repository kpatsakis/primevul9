notify_log(dns_zone_t *zone, int level, const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	dns_zone_logv(zone, DNS_LOGCATEGORY_NOTIFY, level, NULL, fmt, ap);
	va_end(ap);
}