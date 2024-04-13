dns_zone_rpz_enable(dns_zone_t *zone, dns_rpz_zones_t *rpzs,
		    dns_rpz_num_t rpz_num)
{
	/*
	 * Only RBTDB zones can be used for response policy zones,
	 * because only they have the code to load the create the summary data.
	 * Only zones that are loaded instead of mmap()ed create the
	 * summary data and so can be policy zones.
	 */
	if (strcmp(zone->db_argv[0], "rbt") != 0 &&
	    strcmp(zone->db_argv[0], "rbt64") != 0)
		return (ISC_R_NOTIMPLEMENTED);
	if (zone->masterformat == dns_masterformat_map)
		return (ISC_R_NOTIMPLEMENTED);

	/*
	 * This must happen only once or be redundant.
	 */
	LOCK_ZONE(zone);
	if (zone->rpzs != NULL) {
		REQUIRE(zone->rpzs == rpzs && zone->rpz_num == rpz_num);
	} else {
		REQUIRE(zone->rpz_num == DNS_RPZ_INVALID_NUM);
		dns_rpz_attach_rpzs(rpzs, &zone->rpzs);
		zone->rpz_num = rpz_num;
	}
	rpzs->defined |= DNS_RPZ_ZBIT(rpz_num);
	UNLOCK_ZONE(zone);

	return (ISC_R_SUCCESS);
}