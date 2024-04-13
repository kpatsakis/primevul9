dns_zone_isdynamic(dns_zone_t *zone, bool ignore_freeze) {
	REQUIRE(DNS_ZONE_VALID(zone));

	if (zone->type == dns_zone_slave || zone->type == dns_zone_mirror ||
	    zone->type == dns_zone_stub || zone->type == dns_zone_key ||
	    (zone->type == dns_zone_redirect && zone->masters != NULL))
		return (true);

	/* Inline zones are always dynamic. */
	if (zone->type == dns_zone_master && zone->raw != NULL)
		return (true);

	/* If !ignore_freeze, we need check whether updates are disabled.  */
	if (zone->type == dns_zone_master &&
	    (!zone->update_disabled || ignore_freeze) &&
	    ((zone->ssutable != NULL) ||
	     (zone->update_acl != NULL && !dns_acl_isnone(zone->update_acl))))
		return (true);

	return (false);

}