zone_free(dns_zone_t *zone) {
	isc_mem_t *mctx = NULL;
	dns_signing_t *signing;
	dns_nsec3chain_t *nsec3chain;
	isc_event_t *setnsec3param_event;
	dns_include_t *include;

	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(isc_refcount_current(&zone->erefs) == 0);
	REQUIRE(zone->irefs == 0);
	REQUIRE(!LOCKED_ZONE(zone));
	REQUIRE(zone->timer == NULL);
	REQUIRE(zone->zmgr == NULL);

	/*
	 * Managed objects.  Order is important.
	 */
	if (zone->request != NULL) {
		dns_request_destroy(&zone->request); /* XXXMPA */
	}
	INSIST(zone->readio == NULL);
	INSIST(zone->statelist == NULL);
	INSIST(zone->writeio == NULL);

	if (zone->task != NULL) {
		isc_task_detach(&zone->task);
	}
	if (zone->loadtask != NULL) {
		isc_task_detach(&zone->loadtask);
	}
	if (zone->view != NULL) {
		dns_view_weakdetach(&zone->view);
	}
	if (zone->prev_view != NULL) {
		dns_view_weakdetach(&zone->prev_view);
	}

	/* Unmanaged objects */
	while (!ISC_LIST_EMPTY(zone->setnsec3param_queue)) {
		setnsec3param_event = ISC_LIST_HEAD(zone->setnsec3param_queue);
		ISC_LIST_UNLINK(zone->setnsec3param_queue, setnsec3param_event,
				ev_link);
		isc_event_free(&setnsec3param_event);
	}
	for (signing = ISC_LIST_HEAD(zone->signing);
	     signing != NULL;
	     signing = ISC_LIST_HEAD(zone->signing)) {
		ISC_LIST_UNLINK(zone->signing, signing, link);
		dns_db_detach(&signing->db);
		dns_dbiterator_destroy(&signing->dbiterator);
		isc_mem_put(zone->mctx, signing, sizeof *signing);
	}
	for (nsec3chain = ISC_LIST_HEAD(zone->nsec3chain);
	     nsec3chain != NULL;
	     nsec3chain = ISC_LIST_HEAD(zone->nsec3chain)) {
		ISC_LIST_UNLINK(zone->nsec3chain, nsec3chain, link);
		dns_db_detach(&nsec3chain->db);
		dns_dbiterator_destroy(&nsec3chain->dbiterator);
		isc_mem_put(zone->mctx, nsec3chain, sizeof *nsec3chain);
	}
	for (include = ISC_LIST_HEAD(zone->includes);
	     include != NULL;
	     include = ISC_LIST_HEAD(zone->includes)) {
		ISC_LIST_UNLINK(zone->includes, include, link);
		isc_mem_free(zone->mctx, include->name);
		isc_mem_put(zone->mctx, include, sizeof *include);
	}
	for (include = ISC_LIST_HEAD(zone->newincludes);
	     include != NULL;
	     include = ISC_LIST_HEAD(zone->newincludes)) {
		ISC_LIST_UNLINK(zone->newincludes, include, link);
		isc_mem_free(zone->mctx, include->name);
		isc_mem_put(zone->mctx, include, sizeof *include);
	}
	if (zone->masterfile != NULL) {
		isc_mem_free(zone->mctx, zone->masterfile);
	}
	zone->masterfile = NULL;
	if (zone->keydirectory != NULL) {
		isc_mem_free(zone->mctx, zone->keydirectory);
	}
	zone->keydirectory = NULL;
	zone->journalsize = -1;
	if (zone->journal != NULL) {
		isc_mem_free(zone->mctx, zone->journal);
	}
	zone->journal = NULL;
	if (zone->stats != NULL) {
		isc_stats_detach(&zone->stats);
	}
	if (zone->requeststats != NULL) {
		isc_stats_detach(&zone->requeststats);
	}
	if (zone->rcvquerystats != NULL){
		dns_stats_detach(&zone->rcvquerystats);
	}
	if (zone->db != NULL) {
		zone_detachdb(zone);
	}
	if (zone->rpzs != NULL) {
		REQUIRE(zone->rpz_num < zone->rpzs->p.num_zones);
		dns_rpz_detach_rpzs(&zone->rpzs);
		zone->rpz_num = DNS_RPZ_INVALID_NUM;
	}
	if (zone->catzs != NULL) {
		dns_catz_catzs_detach(&zone->catzs);
	}
	zone_freedbargs(zone);
	RUNTIME_CHECK(dns_zone_setmasterswithkeys(zone, NULL,
						  NULL, 0) == ISC_R_SUCCESS);
	RUNTIME_CHECK(dns_zone_setalsonotify(zone, NULL, 0) == ISC_R_SUCCESS);
	zone->check_names = dns_severity_ignore;
	if (zone->update_acl != NULL) {
		dns_acl_detach(&zone->update_acl);
	}
	if (zone->forward_acl != NULL) {
		dns_acl_detach(&zone->forward_acl);
	}
	if (zone->notify_acl != NULL) {
		dns_acl_detach(&zone->notify_acl);
	}
	if (zone->query_acl != NULL) {
		dns_acl_detach(&zone->query_acl);
	}
	if (zone->queryon_acl != NULL) {
		dns_acl_detach(&zone->queryon_acl);
	}
	if (zone->xfr_acl != NULL) {
		dns_acl_detach(&zone->xfr_acl);
	}
	if (dns_name_dynamic(&zone->origin)) {
		dns_name_free(&zone->origin, zone->mctx);
	}
	if (zone->strnamerd != NULL) {
		isc_mem_free(zone->mctx, zone->strnamerd);
	}
	if (zone->strname != NULL) {
		isc_mem_free(zone->mctx, zone->strname);
	}
	if (zone->strrdclass != NULL) {
		isc_mem_free(zone->mctx, zone->strrdclass);
	}
	if (zone->strviewname != NULL) {
		isc_mem_free(zone->mctx, zone->strviewname);
	}
	if (zone->ssutable != NULL) {
		dns_ssutable_detach(&zone->ssutable);
	}
	if (zone->gluecachestats != NULL) {
		isc_stats_detach(&zone->gluecachestats);
	}

	/* last stuff */
	ZONEDB_DESTROYLOCK(&zone->dblock);
	isc_mutex_destroy(&zone->lock);
	zone->magic = 0;
	mctx = zone->mctx;
	isc_mem_put(mctx, zone, sizeof(*zone));
	isc_mem_detach(&mctx);
}