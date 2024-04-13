get_raw_serial(dns_zone_t *raw, dns_masterrawheader_t *rawdata) {
	isc_result_t result;
	unsigned int soacount;

	LOCK(&raw->lock);
	if (raw->db != NULL) {
		result = zone_get_from_db(raw, raw->db, NULL, &soacount,
					  &rawdata->sourceserial,
					  NULL, NULL, NULL, NULL,
					  NULL);
		if (result == ISC_R_SUCCESS && soacount > 0U)
			rawdata->flags |= DNS_MASTERRAW_SOURCESERIALSET;
	}
	UNLOCK(&raw->lock);
}