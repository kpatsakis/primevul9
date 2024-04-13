dns_zone_rpz_enable_db(dns_zone_t *zone, dns_db_t *db) {
	isc_result_t result;
	if (zone->rpz_num == DNS_RPZ_INVALID_NUM)
		return;
	REQUIRE(zone->rpzs != NULL);
	zone->rpzs->zones[zone->rpz_num]->db_registered = true;
	result = dns_db_updatenotify_register(db,
					      dns_rpz_dbupdate_callback,
					      zone->rpzs->zones[zone->rpz_num]);
	REQUIRE(result == ISC_R_SUCCESS);
}