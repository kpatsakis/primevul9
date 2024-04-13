set_refreshkeytimer(dns_zone_t *zone, dns_rdata_keydata_t *key,
		    isc_stdtime_t now, bool force)
{
	const char me[] = "set_refreshkeytimer";
	isc_stdtime_t then;
	isc_time_t timenow, timethen;
	char timebuf[80];

	ENTER;
	then = key->refresh;
	if (force)
		then = now;
	if (key->addhd > now && key->addhd < then)
		then = key->addhd;
	if (key->removehd > now && key->removehd < then)
		then = key->removehd;

	TIME_NOW(&timenow);
	if (then > now)
		DNS_ZONE_TIME_ADD(&timenow, then - now, &timethen);
	else
		timethen = timenow;
	if (isc_time_compare(&zone->refreshkeytime, &timenow) < 0 ||
	    isc_time_compare(&timethen, &zone->refreshkeytime) < 0)
		zone->refreshkeytime = timethen;

	isc_time_formattimestamp(&zone->refreshkeytime, timebuf, 80);
	dns_zone_log(zone, ISC_LOG_DEBUG(1), "next key refresh: %s", timebuf);
	zone_settimer(zone, &timenow);
}