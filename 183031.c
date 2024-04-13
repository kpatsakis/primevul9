get_master_options(dns_zone_t *zone) {
	unsigned int options;

	options = DNS_MASTER_ZONE | DNS_MASTER_RESIGN;
	if (zone->type == dns_zone_slave || zone->type == dns_zone_mirror ||
	    (zone->type == dns_zone_redirect && zone->masters == NULL))
	{
		options |= DNS_MASTER_SLAVE;
	}
	if (zone->type == dns_zone_key) {
		options |= DNS_MASTER_KEY;
	}
	if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_CHECKNS)) {
		options |= DNS_MASTER_CHECKNS;
	}
	if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_FATALNS)) {
		options |= DNS_MASTER_FATALNS;
	}
	if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_CHECKNAMES)) {
		options |= DNS_MASTER_CHECKNAMES;
	}
	if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_CHECKNAMESFAIL)) {
		options |= DNS_MASTER_CHECKNAMESFAIL;
	}
	if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_CHECKMX)) {
		options |= DNS_MASTER_CHECKMX;
	}
	if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_CHECKMXFAIL)) {
		options |= DNS_MASTER_CHECKMXFAIL;
	}
	if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_CHECKWILDCARD)) {
		options |= DNS_MASTER_CHECKWILDCARD;
	}
	if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_CHECKTTL)) {
		options |= DNS_MASTER_CHECKTTL;
	}

	return (options);
}