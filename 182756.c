dns_zonemgr_attach(dns_zonemgr_t *source, dns_zonemgr_t **target) {
	REQUIRE(DNS_ZONEMGR_VALID(source));
	REQUIRE(target != NULL && *target == NULL);

	RWLOCK(&source->rwlock, isc_rwlocktype_write);
	REQUIRE(source->refs > 0);
	source->refs++;
	INSIST(source->refs > 0);
	RWUNLOCK(&source->rwlock, isc_rwlocktype_write);
	*target = source;
}