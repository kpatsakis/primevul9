dns_zone_addnsec3chain(dns_zone_t *zone, dns_rdata_nsec3param_t *nsec3param) {
	isc_result_t result;
	char salt[255*2+1];

	REQUIRE(DNS_ZONE_VALID(zone));

	result = dns_nsec3param_salttotext(nsec3param, salt, sizeof(salt));
	RUNTIME_CHECK(result == ISC_R_SUCCESS);
	dnssec_log(zone, ISC_LOG_NOTICE,
		   "dns_zone_addnsec3chain(hash=%u, iterations=%u, salt=%s)",
		   nsec3param->hash, nsec3param->iterations, salt);
	LOCK_ZONE(zone);
	result = zone_addnsec3chain(zone, nsec3param);
	UNLOCK_ZONE(zone);

	return (result);
}