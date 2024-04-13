zone_iattach(dns_zone_t *source, dns_zone_t **target) {

	/*
	 * 'source' locked by caller.
	 */
	REQUIRE(LOCKED_ZONE(source));
	REQUIRE(DNS_ZONE_VALID(source));
	REQUIRE(target != NULL && *target == NULL);
	INSIST(source->irefs + isc_refcount_current(&source->erefs) > 0);
	source->irefs++;
	INSIST(source->irefs != 0);
	*target = source;
}