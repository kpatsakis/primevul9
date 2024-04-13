dumptostream(dns_zone_t *zone, FILE *fd, const dns_master_style_t *style,
	     dns_masterformat_t format, const uint32_t rawversion)
{
	isc_result_t result;
	dns_dbversion_t *version = NULL;
	dns_db_t *db = NULL;
	dns_masterrawheader_t rawdata;

	REQUIRE(DNS_ZONE_VALID(zone));

	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	if (zone->db != NULL)
		dns_db_attach(zone->db, &db);
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
	if (db == NULL)
		return (DNS_R_NOTLOADED);

	dns_db_currentversion(db, &version);
	dns_master_initrawheader(&rawdata);
	if (rawversion == 0)
		rawdata.flags |= DNS_MASTERRAW_COMPAT;
	else if (inline_secure(zone))
		get_raw_serial(zone->raw, &rawdata);
	else if (zone->sourceserialset) {
		rawdata.flags = DNS_MASTERRAW_SOURCESERIALSET;
		rawdata.sourceserial = zone->sourceserial;
	}
	result = dns_master_dumptostream(zone->mctx, db, version, style,
					 format, &rawdata, fd);
	dns_db_closeversion(db, &version, false);
	dns_db_detach(&db);
	return (result);
}