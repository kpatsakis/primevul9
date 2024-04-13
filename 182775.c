dns_zone_asyncload(dns_zone_t *zone, bool newonly,
		   dns_zt_zoneloaded_t done, void *arg)
{
	isc_event_t *e;
	dns_asyncload_t *asl = NULL;
	isc_result_t result = ISC_R_SUCCESS;

	REQUIRE(DNS_ZONE_VALID(zone));

	if (zone->zmgr == NULL)
		return (ISC_R_FAILURE);

	/* If we already have a load pending, stop now */
	LOCK_ZONE(zone);
	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADPENDING)) {
		UNLOCK_ZONE(zone);
		return (ISC_R_ALREADYRUNNING);
	}

	asl = isc_mem_get(zone->mctx, sizeof (*asl));
	if (asl == NULL)
		CHECK(ISC_R_NOMEMORY);

	asl->zone = NULL;
	asl->flags = newonly ? DNS_ZONELOADFLAG_NOSTAT : 0;
	asl->loaded = done;
	asl->loaded_arg = arg;

	e = isc_event_allocate(zone->zmgr->mctx, zone->zmgr,
			       DNS_EVENT_ZONELOAD,
			       zone_asyncload, asl,
			       sizeof(isc_event_t));
	if (e == NULL)
		CHECK(ISC_R_NOMEMORY);

	zone_iattach(zone, &asl->zone);
	DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_LOADPENDING);
	isc_task_send(zone->loadtask, &e);
	UNLOCK_ZONE(zone);

	return (ISC_R_SUCCESS);

  failure:
	if (asl != NULL)
		isc_mem_put(zone->mctx, asl, sizeof (*asl));
	UNLOCK_ZONE(zone);
	return (result);
}