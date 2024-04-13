dns_zone_create(dns_zone_t **zonep, isc_mem_t *mctx) {
	isc_result_t result;
	dns_zone_t *zone;
	isc_time_t now;

	REQUIRE(zonep != NULL && *zonep == NULL);
	REQUIRE(mctx != NULL);

	TIME_NOW(&now);
	zone = isc_mem_get(mctx, sizeof(*zone));
	if (zone == NULL) {
		return (ISC_R_NOMEMORY);
	}

	zone->mctx = NULL;
	isc_mem_attach(mctx, &zone->mctx);

	isc_mutex_init(&zone->lock);

	result = ZONEDB_INITLOCK(&zone->dblock);
	if (result != ISC_R_SUCCESS) {
		goto free_mutex;
	}

	/* XXX MPA check that all elements are initialised */
#ifdef DNS_ZONE_CHECKLOCK
	zone->locked = false;
#endif
	zone->db = NULL;
	zone->zmgr = NULL;
	ISC_LINK_INIT(zone, link);
	isc_refcount_init(&zone->erefs, 1);	/* Implicit attach. */
	zone->irefs = 0;
	dns_name_init(&zone->origin, NULL);
	zone->strnamerd = NULL;
	zone->strname = NULL;
	zone->strrdclass = NULL;
	zone->strviewname = NULL;
	zone->masterfile = NULL;
	ISC_LIST_INIT(zone->includes);
	ISC_LIST_INIT(zone->newincludes);
	zone->nincludes = 0;
	zone->masterformat = dns_masterformat_none;
	zone->masterstyle = NULL;
	zone->keydirectory = NULL;
	zone->journalsize = -1;
	zone->journal = NULL;
	zone->rdclass = dns_rdataclass_none;
	zone->type = dns_zone_none;
	zone->flags = 0;
	zone->options = 0;
	zone->keyopts = 0;
	zone->db_argc = 0;
	zone->db_argv = NULL;
	isc_time_settoepoch(&zone->expiretime);
	isc_time_settoepoch(&zone->refreshtime);
	isc_time_settoepoch(&zone->dumptime);
	isc_time_settoepoch(&zone->loadtime);
	zone->notifytime = now;
	isc_time_settoepoch(&zone->resigntime);
	isc_time_settoepoch(&zone->keywarntime);
	isc_time_settoepoch(&zone->signingtime);
	isc_time_settoepoch(&zone->nsec3chaintime);
	isc_time_settoepoch(&zone->refreshkeytime);
	zone->refreshkeyinterval = 0;
	zone->refreshkeycount = 0;
	zone->refresh = DNS_ZONE_DEFAULTREFRESH;
	zone->retry = DNS_ZONE_DEFAULTRETRY;
	zone->expire = 0;
	zone->minimum = 0;
	zone->maxrefresh = DNS_ZONE_MAXREFRESH;
	zone->minrefresh = DNS_ZONE_MINREFRESH;
	zone->maxretry = DNS_ZONE_MAXRETRY;
	zone->minretry = DNS_ZONE_MINRETRY;
	zone->masters = NULL;
	zone->masterdscps = NULL;
	zone->masterkeynames = NULL;
	zone->mastersok = NULL;
	zone->masterscnt = 0;
	zone->curmaster = 0;
	zone->maxttl = 0;
	zone->notify = NULL;
	zone->notifykeynames = NULL;
	zone->notifydscp = NULL;
	zone->notifytype = dns_notifytype_yes;
	zone->notifycnt = 0;
	zone->task = NULL;
	zone->loadtask = NULL;
	zone->update_acl = NULL;
	zone->forward_acl = NULL;
	zone->notify_acl = NULL;
	zone->query_acl = NULL;
	zone->queryon_acl = NULL;
	zone->xfr_acl = NULL;
	zone->update_disabled = false;
	zone->zero_no_soa_ttl = true;
	zone->check_names = dns_severity_ignore;
	zone->request = NULL;
	zone->lctx = NULL;
	zone->readio = NULL;
	zone->dctx = NULL;
	zone->writeio = NULL;
	zone->timer = NULL;
	zone->idlein = DNS_DEFAULT_IDLEIN;
	zone->idleout = DNS_DEFAULT_IDLEOUT;
	zone->log_key_expired_timer = 0;
	ISC_LIST_INIT(zone->notifies);
	isc_sockaddr_any(&zone->notifysrc4);
	isc_sockaddr_any6(&zone->notifysrc6);
	isc_sockaddr_any(&zone->xfrsource4);
	isc_sockaddr_any6(&zone->xfrsource6);
	isc_sockaddr_any(&zone->altxfrsource4);
	isc_sockaddr_any6(&zone->altxfrsource6);
	zone->notifysrc4dscp = -1;
	zone->notifysrc6dscp = -1;
	zone->xfrsource4dscp = -1;
	zone->xfrsource6dscp = -1;
	zone->altxfrsource4dscp = -1;
	zone->altxfrsource6dscp = -1;
	zone->xfr = NULL;
	zone->tsigkey = NULL;
	zone->maxxfrin = MAX_XFER_TIME;
	zone->maxxfrout = MAX_XFER_TIME;
	zone->ssutable = NULL;
	zone->sigvalidityinterval = 30 * 24 * 3600;
	zone->keyvalidityinterval = 0;
	zone->sigresigninginterval = 7 * 24 * 3600;
	zone->view = NULL;
	zone->prev_view = NULL;
	zone->checkmx = NULL;
	zone->checksrv = NULL;
	zone->checkns = NULL;
	ISC_LINK_INIT(zone, statelink);
	zone->statelist = NULL;
	zone->stats = NULL;
	zone->requeststats_on = false;
	zone->statlevel = dns_zonestat_none;
	zone->requeststats = NULL;
	zone->rcvquerystats = NULL;
	zone->notifydelay = 5;
	zone->isself = NULL;
	zone->isselfarg = NULL;
	ISC_LIST_INIT(zone->signing);
	ISC_LIST_INIT(zone->nsec3chain);
	ISC_LIST_INIT(zone->setnsec3param_queue);
	zone->signatures = 10;
	zone->nodes = 100;
	zone->privatetype = (dns_rdatatype_t)0xffffU;
	zone->added = false;
	zone->automatic = false;
	zone->rpzs = NULL;
	zone->rpz_num = DNS_RPZ_INVALID_NUM;

	zone->catzs = NULL;
	zone->parentcatz = NULL;

	ISC_LIST_INIT(zone->forwards);
	zone->raw = NULL;
	zone->secure = NULL;
	zone->sourceserial = 0;
	zone->sourceserialset = false;
	zone->requestixfr = true;
	zone->requestexpire = true;
	ISC_LIST_INIT(zone->rss_events);
	zone->rss_db = NULL;
	zone->rss_raw = NULL;
	zone->rss_newver = NULL;
	zone->rss_oldver = NULL;
	zone->rss_event = NULL;
	zone->rss_state = NULL;
	zone->updatemethod = dns_updatemethod_increment;
	zone->maxrecords = 0U;

	zone->magic = ZONE_MAGIC;

	zone->gluecachestats = NULL;
	result = isc_stats_create(mctx, &zone->gluecachestats,
				  dns_gluecachestatscounter_max);
	if (result != ISC_R_SUCCESS) {
		goto free_erefs;
	}

	/* Must be after magic is set. */
	result = dns_zone_setdbtype(zone, dbargc_default, dbargv_default);
	if (result != ISC_R_SUCCESS) {
		goto free_stats;
	}

	ISC_EVENT_INIT(&zone->ctlevent, sizeof(zone->ctlevent), 0, NULL,
		       DNS_EVENT_ZONECONTROL, zone_shutdown, zone, zone,
		       NULL, NULL);
	*zonep = zone;
	return (ISC_R_SUCCESS);

 free_stats:
	if (zone->gluecachestats != NULL)
		isc_stats_detach(&zone->gluecachestats);

 free_erefs:
	INSIST(isc_refcount_decrement(&zone->erefs) > 0);
	isc_refcount_destroy(&zone->erefs);

	ZONEDB_DESTROYLOCK(&zone->dblock);

 free_mutex:
	isc_mutex_destroy(&zone->lock);

	isc_mem_putanddetach(&zone->mctx, zone, sizeof(*zone));
	return (result);
}