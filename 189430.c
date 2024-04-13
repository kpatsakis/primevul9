checknames(dns_zonetype_t ztype, const cfg_obj_t **maps,
	   const cfg_obj_t **objp)
{
	const char *zone = NULL;
	isc_result_t result;

	switch (ztype) {
	case dns_zone_slave: zone = "slave"; break;
	case dns_zone_master: zone = "master"; break;
	default:
		INSIST(0);
		ISC_UNREACHABLE();
	}
	result = ns_checknames_get(maps, zone, objp);
	INSIST(result == ISC_R_SUCCESS && objp != NULL && *objp != NULL);
}