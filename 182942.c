dns_zone_next(dns_zone_t *zone, dns_zone_t **next) {
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(next != NULL && *next == NULL);

	*next = ISC_LIST_NEXT(zone, link);
	if (*next == NULL)
		return (ISC_R_NOMORE);
	else
		return (ISC_R_SUCCESS);
}