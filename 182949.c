notify_createmessage(dns_zone_t *zone, unsigned int flags,
		     dns_message_t **messagep)
{
	dns_db_t *zonedb = NULL;
	dns_dbnode_t *node = NULL;
	dns_dbversion_t *version = NULL;
	dns_message_t *message = NULL;
	dns_rdataset_t rdataset;
	dns_rdata_t rdata = DNS_RDATA_INIT;

	dns_name_t *tempname = NULL;
	dns_rdata_t *temprdata = NULL;
	dns_rdatalist_t *temprdatalist = NULL;
	dns_rdataset_t *temprdataset = NULL;

	isc_result_t result;
	isc_region_t r;
	isc_buffer_t *b = NULL;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(messagep != NULL && *messagep == NULL);

	result = dns_message_create(zone->mctx, DNS_MESSAGE_INTENTRENDER,
				    &message);
	if (result != ISC_R_SUCCESS)
		return (result);

	message->opcode = dns_opcode_notify;
	message->flags |= DNS_MESSAGEFLAG_AA;
	message->rdclass = zone->rdclass;

	result = dns_message_gettempname(message, &tempname);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	result = dns_message_gettemprdataset(message, &temprdataset);
	if (result != ISC_R_SUCCESS)
		goto cleanup;

	/*
	 * Make question.
	 */
	dns_name_init(tempname, NULL);
	dns_name_clone(&zone->origin, tempname);
	dns_rdataset_makequestion(temprdataset, zone->rdclass,
				  dns_rdatatype_soa);
	ISC_LIST_APPEND(tempname->list, temprdataset, link);
	dns_message_addname(message, tempname, DNS_SECTION_QUESTION);
	tempname = NULL;
	temprdataset = NULL;

	if ((flags & DNS_NOTIFY_NOSOA) != 0)
		goto done;

	result = dns_message_gettempname(message, &tempname);
	if (result != ISC_R_SUCCESS)
		goto soa_cleanup;
	result = dns_message_gettemprdata(message, &temprdata);
	if (result != ISC_R_SUCCESS)
		goto soa_cleanup;
	result = dns_message_gettemprdataset(message, &temprdataset);
	if (result != ISC_R_SUCCESS)
		goto soa_cleanup;
	result = dns_message_gettemprdatalist(message, &temprdatalist);
	if (result != ISC_R_SUCCESS)
		goto soa_cleanup;

	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	INSIST(zone->db != NULL); /* XXXJT: is this assumption correct? */
	dns_db_attach(zone->db, &zonedb);
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);

	dns_name_init(tempname, NULL);
	dns_name_clone(&zone->origin, tempname);
	dns_db_currentversion(zonedb, &version);
	result = dns_db_findnode(zonedb, tempname, false, &node);
	if (result != ISC_R_SUCCESS)
		goto soa_cleanup;

	dns_rdataset_init(&rdataset);
	result = dns_db_findrdataset(zonedb, node, version,
				     dns_rdatatype_soa,
				     dns_rdatatype_none, 0, &rdataset,
				     NULL);
	if (result != ISC_R_SUCCESS)
		goto soa_cleanup;
	result = dns_rdataset_first(&rdataset);
	if (result != ISC_R_SUCCESS)
		goto soa_cleanup;
	dns_rdataset_current(&rdataset, &rdata);
	dns_rdata_toregion(&rdata, &r);
	result = isc_buffer_allocate(zone->mctx, &b, r.length);
	if (result != ISC_R_SUCCESS)
		goto soa_cleanup;
	isc_buffer_putmem(b, r.base, r.length);
	isc_buffer_usedregion(b, &r);
	dns_rdata_init(temprdata);
	dns_rdata_fromregion(temprdata, rdata.rdclass, rdata.type, &r);
	dns_message_takebuffer(message, &b);
	result = dns_rdataset_next(&rdataset);
	dns_rdataset_disassociate(&rdataset);
	if (result != ISC_R_NOMORE)
		goto soa_cleanup;
	temprdatalist->rdclass = rdata.rdclass;
	temprdatalist->type = rdata.type;
	temprdatalist->ttl = rdataset.ttl;
	ISC_LIST_APPEND(temprdatalist->rdata, temprdata, link);

	result = dns_rdatalist_tordataset(temprdatalist, temprdataset);
	if (result != ISC_R_SUCCESS)
		goto soa_cleanup;

	ISC_LIST_APPEND(tempname->list, temprdataset, link);
	dns_message_addname(message, tempname, DNS_SECTION_ANSWER);
	temprdatalist = NULL;
	temprdataset = NULL;
	temprdata = NULL;
	tempname = NULL;

 soa_cleanup:
	if (node != NULL)
		dns_db_detachnode(zonedb, &node);
	if (version != NULL)
		dns_db_closeversion(zonedb, &version, false);
	if (zonedb != NULL)
		dns_db_detach(&zonedb);
	if (tempname != NULL)
		dns_message_puttempname(message, &tempname);
	if (temprdata != NULL)
		dns_message_puttemprdata(message, &temprdata);
	if (temprdataset != NULL)
		dns_message_puttemprdataset(message, &temprdataset);
	if (temprdatalist != NULL)
		dns_message_puttemprdatalist(message, &temprdatalist);

 done:
	*messagep = message;
	return (ISC_R_SUCCESS);

 cleanup:
	if (tempname != NULL)
		dns_message_puttempname(message, &tempname);
	if (temprdataset != NULL)
		dns_message_puttemprdataset(message, &temprdataset);
	dns_message_destroy(&message);
	return (result);
}