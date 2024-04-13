dns_zone_dialup(dns_zone_t *zone) {

	REQUIRE(DNS_ZONE_VALID(zone));

	zone_debuglog(zone, "dns_zone_dialup", 3,
		      "notify = %d, refresh = %d",
		      DNS_ZONE_FLAG(zone, DNS_ZONEFLG_DIALNOTIFY),
		      DNS_ZONE_FLAG(zone, DNS_ZONEFLG_DIALREFRESH));

	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_DIALNOTIFY))
		dns_zone_notify(zone);
	if (zone->type != dns_zone_master && zone->masters != NULL &&
	    DNS_ZONE_FLAG(zone, DNS_ZONEFLG_DIALREFRESH))
		dns_zone_refresh(zone);
}