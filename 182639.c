dnssec_report(const char *format, ...) {
	va_list args;
	va_start(args, format);
	isc_log_vwrite(dns_lctx, DNS_LOGCATEGORY_DNSSEC, DNS_LOGMODULE_ZONE,
		       ISC_LOG_INFO, format, args);
	va_end(args);
}