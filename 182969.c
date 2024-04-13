zone_load(dns_zone_t *zone, unsigned int flags, bool locked) {
	isc_result_t result;
	isc_time_t now;
	isc_time_t loadtime;
	dns_db_t *db = NULL;
	bool rbt, hasraw;

	REQUIRE(DNS_ZONE_VALID(zone));

	if (!locked)
		LOCK_ZONE(zone);

	INSIST(zone != zone->raw);
	hasraw = inline_secure(zone);
	if (hasraw) {
		/*
		 * We are trying to load an inline-signed zone.  First call
		 * self recursively to try loading the raw version of the zone.
		 * Assuming the raw zone file is readable, there are two
		 * possibilities:
		 *
		 *  a) the raw zone was not yet loaded and thus it will be
		 *     loaded now, synchronously; if this succeeds, a
		 *     subsequent attempt to load the signed zone file will
		 *     take place and thus zone_postload() will be called
		 *     twice: first for the raw zone and then for the secure
		 *     zone; the latter call will take care of syncing the raw
		 *     version with the secure version,
		 *
		 *  b) the raw zone was already loaded and we are trying to
		 *     reload it, which will happen asynchronously; this means
		 *     zone_postload() will only be called for the raw zone
		 *     because "result" returned by the zone_load() call below
		 *     will not be ISC_R_SUCCESS but rather DNS_R_CONTINUE;
		 *     zone_postload() called for the raw zone will take care
		 *     of syncing the raw version with the secure version.
		 */
		result = zone_load(zone->raw, flags, false);
		if (result != ISC_R_SUCCESS) {
			if (!locked)
				UNLOCK_ZONE(zone);
			return(result);
		}
		LOCK_ZONE(zone->raw);
	}

	TIME_NOW(&now);

	INSIST(zone->type != dns_zone_none);

	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADING)) {
		if ((flags & DNS_ZONELOADFLAG_THAW) != 0)
			DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_THAW);
		result = DNS_R_CONTINUE;
		goto cleanup;
	}

	INSIST(zone->db_argc >= 1);

	rbt = strcmp(zone->db_argv[0], "rbt") == 0 ||
	      strcmp(zone->db_argv[0], "rbt64") == 0;

	if (zone->db != NULL && zone->masterfile == NULL && rbt) {
		/*
		 * The zone has no master file configured.
		 */
		result = ISC_R_SUCCESS;
		goto cleanup;
	}

	if (zone->db != NULL && dns_zone_isdynamic(zone, false)) {
		/*
		 * This is a slave, stub, or dynamically updated
		 * zone being reloaded.  Do nothing - the database
		 * we already have is guaranteed to be up-to-date.
		 */
		if (zone->type == dns_zone_master && !hasraw)
			result = DNS_R_DYNAMIC;
		else
			result = ISC_R_SUCCESS;
		goto cleanup;
	}

	/*
	 * Store the current time before the zone is loaded, so that if the
	 * file changes between the time of the load and the time that
	 * zone->loadtime is set, then the file will still be reloaded
	 * the next time dns_zone_load is called.
	 */
	TIME_NOW(&loadtime);

	/*
	 * Don't do the load if the file that stores the zone is older
	 * than the last time the zone was loaded.  If the zone has not
	 * been loaded yet, zone->loadtime will be the epoch.
	 */
	if (zone->masterfile != NULL) {
		isc_time_t filetime;

		/*
		 * The file is already loaded.	If we are just doing a
		 * "rndc reconfig", we are done.
		 */
		if (!isc_time_isepoch(&zone->loadtime) &&
		    (flags & DNS_ZONELOADFLAG_NOSTAT) != 0) {
			result = ISC_R_SUCCESS;
			goto cleanup;
		}

		if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADED) &&
		    !zone_touched(zone))
		{
			dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
				      ISC_LOG_DEBUG(1),
				      "skipping load: master file "
				      "older than last load");
			result = DNS_R_UPTODATE;
			goto cleanup;
		}

		/*
		 * If the file modification time is in the past
		 * set loadtime to that value.
		 */
		result = isc_file_getmodtime(zone->masterfile, &filetime);
		if (result == ISC_R_SUCCESS &&
		    isc_time_compare(&loadtime, &filetime) > 0)
			loadtime = filetime;
	}

	/*
	 * Built in zones (with the exception of empty zones) don't need
	 * to be reloaded.
	 */
	if (zone->type == dns_zone_master &&
	    strcmp(zone->db_argv[0], "_builtin") == 0 &&
	    (zone->db_argc < 2 || strcmp(zone->db_argv[1], "empty") != 0) &&
	    DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADED)) {
		result = ISC_R_SUCCESS;
		goto cleanup;
	}

	/*
	 * Zones associated with a DLZ don't need to be loaded either,
	 * but we need to associate the database with the zone object.
	 */
	if (strcmp(zone->db_argv[0], "dlz") == 0) {
		dns_dlzdb_t *dlzdb;
		dns_dlzfindzone_t findzone;

		for (dlzdb = ISC_LIST_HEAD(zone->view->dlz_unsearched);
		     dlzdb != NULL;
		     dlzdb = ISC_LIST_NEXT(dlzdb, link))
		{
			INSIST(DNS_DLZ_VALID(dlzdb));
			if (strcmp(zone->db_argv[1], dlzdb->dlzname) == 0)
				break;
		}

		if (dlzdb == NULL) {
			dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
				      ISC_LOG_ERROR,
				      "DLZ %s does not exist or is set "
				      "to 'search yes;'", zone->db_argv[1]);
			result = ISC_R_NOTFOUND;
			goto cleanup;
		}

		ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_write);
		/* ask SDLZ driver if the zone is supported */
		findzone = dlzdb->implementation->methods->findzone;
		result = (*findzone)(dlzdb->implementation->driverarg,
				     dlzdb->dbdata, dlzdb->mctx,
				     zone->view->rdclass, &zone->origin,
				     NULL, NULL, &db);
		if (result != ISC_R_NOTFOUND) {
			if (zone->db != NULL)
				zone_detachdb(zone);
			zone_attachdb(zone, db);
			dns_db_detach(&db);
			result = ISC_R_SUCCESS;
		}
		ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_write);

		if (result == ISC_R_SUCCESS) {
			if (dlzdb->configure_callback == NULL)
				goto cleanup;

			result = (*dlzdb->configure_callback)(zone->view,
							      dlzdb, zone);
			if (result != ISC_R_SUCCESS)
				dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
					      ISC_LOG_ERROR,
					      "DLZ configuration callback: %s",
					      isc_result_totext(result));
		}
		goto cleanup;
	}

	if ((zone->type == dns_zone_slave || zone->type == dns_zone_mirror ||
	     zone->type == dns_zone_stub ||
	     (zone->type == dns_zone_redirect && zone->masters != NULL)) &&
	    rbt) {
		if (zone->masterfile == NULL ||
		    !isc_file_exists(zone->masterfile)) {
			if (zone->masterfile != NULL) {
				dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
					      ISC_LOG_DEBUG(1),
					     "no master file");
			}
			zone->refreshtime = now;
			if (zone->task != NULL)
				zone_settimer(zone, &now);
			result = ISC_R_SUCCESS;
			goto cleanup;
		}
	}

	dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
		      ISC_LOG_DEBUG(1), "starting load");

	result = dns_db_create(zone->mctx, zone->db_argv[0],
			       &zone->origin, (zone->type == dns_zone_stub) ?
			       dns_dbtype_stub : dns_dbtype_zone,
			       zone->rdclass,
			       zone->db_argc - 1, zone->db_argv + 1,
			       &db);

	if (result != ISC_R_SUCCESS) {
		dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD, ISC_LOG_ERROR,
			     "loading zone: creating database: %s",
			     isc_result_totext(result));
		goto cleanup;
	}
	dns_db_settask(db, zone->task);

	if (zone->type == dns_zone_master || zone->type == dns_zone_slave ||
	    zone->type == dns_zone_mirror)
	{
		result = dns_db_setgluecachestats(db, zone->gluecachestats);
		if (result == ISC_R_NOTIMPLEMENTED) {
			result = ISC_R_SUCCESS;
		}
		if (result != ISC_R_SUCCESS) {
			goto cleanup;
		}
	}

	if (! dns_db_ispersistent(db)) {
		if (zone->masterfile != NULL) {
			result = zone_startload(db, zone, loadtime);
		} else {
			result = DNS_R_NOMASTERFILE;
			if (zone->type == dns_zone_master ||
			    (zone->type == dns_zone_redirect &&
			     zone->masters == NULL)) {
				dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
					      ISC_LOG_ERROR,
					     "loading zone: "
					     "no master file configured");
				goto cleanup;
			}
			dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
				      ISC_LOG_INFO, "loading zone: "
				      "no master file configured: continuing");
		}
	}

	if (result == DNS_R_CONTINUE) {
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_LOADING);
		if ((flags & DNS_ZONELOADFLAG_THAW) != 0)
			DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_THAW);
		goto cleanup;
	}

	result = zone_postload(zone, db, loadtime, result);

 cleanup:
	if (hasraw)
		UNLOCK_ZONE(zone->raw);
	if (!locked)
		UNLOCK_ZONE(zone);
	if (db != NULL)
		dns_db_detach(&db);
	return (result);
}