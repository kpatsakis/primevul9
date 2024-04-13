fail_secure(dns_zone_t *zone, dns_name_t *keyname) {
	isc_result_t result;
	dns_keytable_t *sr = NULL;

	result = dns_view_getsecroots(zone->view, &sr);
	if (result == ISC_R_SUCCESS) {
		dns_keytable_marksecure(sr, keyname);
		dns_keytable_detach(&sr);
	}
}