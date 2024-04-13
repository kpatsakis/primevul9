dns_zone_logv(dns_zone_t *zone, isc_logcategory_t *category, int level,
	      const char *prefix, const char *fmt, va_list ap)
{
	char message[4096];
	const char *zstr;

	if (!isc_log_wouldlog(dns_lctx, level)) {
		return;
	}

	vsnprintf(message, sizeof(message), fmt, ap);

	switch (zone->type) {
	case dns_zone_key:
		zstr = "managed-keys-zone";
		break;
	case dns_zone_redirect:
		zstr = "redirect-zone";
		break;
	default:
		zstr = "zone ";
	}

	isc_log_write(dns_lctx, category, DNS_LOGMODULE_ZONE, level,
		      "%s%s%s%s: %s",
		      (prefix != NULL ? prefix : ""),
		      (prefix != NULL ? ": " : ""),
		      zstr, zone->strnamerd, message);
}