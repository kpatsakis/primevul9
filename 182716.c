dns_zone_setnsec3param(dns_zone_t *zone, uint8_t hash, uint8_t flags,
		       uint16_t iter, uint8_t saltlen,
		       unsigned char *salt, bool replace)
{
	isc_result_t result = ISC_R_SUCCESS;
	dns_rdata_nsec3param_t param;
	dns_rdata_t nrdata = DNS_RDATA_INIT;
	dns_rdata_t prdata = DNS_RDATA_INIT;
	unsigned char nbuf[DNS_NSEC3PARAM_BUFFERSIZE];
	struct np3event *npe;
	nsec3param_t *np;
	dns_zone_t *dummy = NULL;
	isc_buffer_t b;
	isc_event_t *e;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(salt != NULL);

	LOCK_ZONE(zone);

	e = isc_event_allocate(zone->mctx, zone, DNS_EVENT_SETNSEC3PARAM,
			       setnsec3param, zone, sizeof(struct np3event));
	if (e == NULL) {
		result = ISC_R_NOMEMORY;
		goto failure;
	}

	npe = (struct np3event *) e;
	np = &npe->params;

	np->replace = replace;
	if (hash == 0) {
		np->length = 0;
		np->nsec = true;
	} else {
		param.common.rdclass = zone->rdclass;
		param.common.rdtype = dns_rdatatype_nsec3param;
		ISC_LINK_INIT(&param.common, link);
		param.mctx = NULL;
		param.hash = hash;
		param.flags = flags;
		param.iterations = iter;
		param.salt_length = saltlen;
		param.salt = salt;
		isc_buffer_init(&b, nbuf, sizeof(nbuf));
		CHECK(dns_rdata_fromstruct(&nrdata, zone->rdclass,
					   dns_rdatatype_nsec3param,
					   &param, &b));
		dns_nsec3param_toprivate(&nrdata, &prdata, zone->privatetype,
					 np->data, sizeof(np->data));
		np->length = prdata.length;
		np->nsec = false;
	}

	/*
	 * setnsec3param() will silently return early if the zone does not yet
	 * have a database.  Prevent that by queueing the event up if zone->db
	 * is NULL.  All events queued here are subsequently processed by
	 * receive_secure_db() if it ever gets called or simply freed by
	 * zone_free() otherwise.
	 */
	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
	if (zone->db != NULL) {
		zone_iattach(zone, &dummy);
		isc_task_send(zone->task, &e);
	} else {
		ISC_LIST_APPEND(zone->setnsec3param_queue, e, ev_link);
		e = NULL;
	}
	ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);

 failure:
	if (e != NULL) {
		isc_event_free(&e);
	}
	UNLOCK_ZONE(zone);
	return (result);
}