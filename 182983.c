set_key_expiry_warning(dns_zone_t *zone, isc_stdtime_t when, isc_stdtime_t now)
{
	unsigned int delta;
	char timebuf[80];

	zone->key_expiry = when;
	if (when <= now) {
		dns_zone_log(zone, ISC_LOG_ERROR,
			     "DNSKEY RRSIG(s) have expired");
		isc_time_settoepoch(&zone->keywarntime);
	} else if (when < now + 7 * 24 * 3600) {
		isc_time_t t;
		isc_time_set(&t, when, 0);
		isc_time_formattimestamp(&t, timebuf, 80);
		dns_zone_log(zone, ISC_LOG_WARNING,
			     "DNSKEY RRSIG(s) will expire within 7 days: %s",
			     timebuf);
		delta = when - now;
		delta--;		/* loop prevention */
		delta /= 24 * 3600;	/* to whole days */
		delta *= 24 * 3600;	/* to seconds */
		isc_time_set(&zone->keywarntime, when - delta, 0);
	}  else {
		isc_time_set(&zone->keywarntime, when - 7 * 24 * 3600, 0);
		isc_time_formattimestamp(&zone->keywarntime, timebuf, 80);
		dns_zone_log(zone, ISC_LOG_NOTICE,
			     "setting keywarntime to %s", timebuf);
	}
}