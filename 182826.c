add_chains(dns_zone_t *zone, dns_db_t *db, dns_dbversion_t *ver,
	   dns_diff_t *diff)
{
	dns_name_t *origin;
	bool build_nsec3;
	isc_result_t result;

	origin = dns_db_origin(db);
	CHECK(dns_private_chains(db, ver, zone->privatetype, NULL,
				 &build_nsec3));
	if (build_nsec3)
		CHECK(dns_nsec3_addnsec3sx(db, ver, origin, zone->minimum,
					   false, zone->privatetype, diff));
	CHECK(updatesecure(db, ver, origin, zone->minimum, true, diff));

 failure:
	return (result);
}