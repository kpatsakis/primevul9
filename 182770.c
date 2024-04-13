dns_zone_setaltxfrsource4(dns_zone_t *zone,
			  const isc_sockaddr_t *altxfrsource)
{
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	zone->altxfrsource4 = *altxfrsource;
	UNLOCK_ZONE(zone);

	return (ISC_R_SUCCESS);
}