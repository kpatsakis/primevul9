dns_zone_signwithkey(dns_zone_t *zone, dns_secalg_t algorithm,
		     uint16_t keyid, bool deleteit)
{
	isc_result_t result;
	REQUIRE(DNS_ZONE_VALID(zone));

	dnssec_log(zone, ISC_LOG_NOTICE,
		   "dns_zone_signwithkey(algorithm=%u, keyid=%u)",
		   algorithm, keyid);
	LOCK_ZONE(zone);
	result = zone_signwithkey(zone, algorithm, keyid, deleteit);
	UNLOCK_ZONE(zone);

	return (result);
}