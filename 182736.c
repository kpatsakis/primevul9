zone_postload(dns_zone_t *zone, dns_db_t *db, isc_time_t loadtime,
	      isc_result_t result)
{
	unsigned int soacount = 0;
	unsigned int nscount = 0;
	unsigned int errors = 0;
	uint32_t serial, oldserial, refresh, retry, expire, minimum;
	isc_time_t now;
	bool needdump = false;
	bool hasinclude = DNS_ZONE_FLAG(zone, DNS_ZONEFLG_HASINCLUDE);
	bool nomaster = false;
	bool had_db = false;
	unsigned int options;
	dns_include_t *inc;

	INSIST(LOCKED_ZONE(zone));
	if (inline_raw(zone)) {
		INSIST(LOCKED_ZONE(zone->secure));
	}

	TIME_NOW(&now);

	/*
	 * Initiate zone transfer?  We may need a error code that
	 * indicates that the "permanent" form does not exist.
	 * XXX better error feedback to log.
	 */
	if (result != ISC_R_SUCCESS && result != DNS_R_SEENINCLUDE) {
		if (zone->type == dns_zone_slave ||
		    zone->type == dns_zone_mirror ||
		    zone->type == dns_zone_stub ||
		    (zone->type == dns_zone_redirect &&
		     zone->masters == NULL))
		{
			if (result == ISC_R_FILENOTFOUND) {
				dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
					      ISC_LOG_DEBUG(1),
					     "no master file");
			} else if (result != DNS_R_NOMASTERFILE) {
				dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
					      ISC_LOG_ERROR,
					     "loading from master file %s "
					     "failed: %s",
					     zone->masterfile,
					     dns_result_totext(result));
			}
		} else if (zone->type == dns_zone_master &&
			   inline_secure(zone) && result == ISC_R_FILENOTFOUND)
		{
			dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
				      ISC_LOG_DEBUG(1),
				     "no master file, requesting db");
			maybe_send_secure(zone);
		} else {
			int level = ISC_LOG_ERROR;
			if (zone->type == dns_zone_key &&
			    result == ISC_R_FILENOTFOUND)
				level = ISC_LOG_DEBUG(1);
			dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD, level,
				     "loading from master file %s failed: %s",
				     zone->masterfile,
				     dns_result_totext(result));
			nomaster = true;
		}

		if (zone->type != dns_zone_key) {
			goto cleanup;
		}
	}

	dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD, ISC_LOG_DEBUG(2),
		     "number of nodes in database: %u",
		     dns_db_nodecount(db));

	if (result == DNS_R_SEENINCLUDE) {
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_HASINCLUDE);
	} else {
		DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_HASINCLUDE);
	}

	/*
	 * If there's no master file for a key zone, then the zone is new:
	 * create an SOA record.  (We do this now, instead of later, so that
	 * if there happens to be a journal file, we can roll forward from
	 * a sane starting point.)
	 */
	if (nomaster && zone->type == dns_zone_key) {
		result = add_soa(zone, db);
		if (result != ISC_R_SUCCESS) {
			goto cleanup;
		}
	}

	/*
	 * Apply update log, if any, on initial load.
	 */
	if (zone->journal != NULL &&
	    ! DNS_ZONE_OPTION(zone, DNS_ZONEOPT_NOMERGE) &&
	    ! DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADED))
	{
		if (zone->type == dns_zone_master && (inline_secure(zone) ||
		    (zone->update_acl != NULL || zone->ssutable != NULL)))
		{
			options = DNS_JOURNALOPT_RESIGN;
		} else {
			options = 0;
		}
		result = dns_journal_rollforward(zone->mctx, db, options,
						 zone->journal);
		if (result != ISC_R_SUCCESS && result != ISC_R_NOTFOUND &&
		    result != DNS_R_UPTODATE && result != DNS_R_NOJOURNAL &&
		    result != ISC_R_RANGE)
		{
			dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
				      ISC_LOG_ERROR,
				     "journal rollforward failed: %s",
				     dns_result_totext(result));
			goto cleanup;


		}
		if (result == ISC_R_NOTFOUND || result == ISC_R_RANGE) {
			dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
				      ISC_LOG_ERROR,
				     "journal rollforward failed: "
				     "journal out of sync with zone");
			goto cleanup;
		}
		dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD, ISC_LOG_DEBUG(1),
			     "journal rollforward completed "
			     "successfully: %s",
			     dns_result_totext(result));
		if (result == ISC_R_SUCCESS) {
			needdump = true;
		}
	}

	/*
	 * Obtain ns, soa and cname counts for top of zone.
	 */
	INSIST(db != NULL);
	result = zone_get_from_db(zone, db, &nscount, &soacount, &serial,
				  &refresh, &retry, &expire, &minimum,
				  &errors);
	if (result != ISC_R_SUCCESS && zone->type != dns_zone_key) {
		dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD, ISC_LOG_ERROR,
			     "could not find NS and/or SOA records");
	}

	/*
	 * Check to make sure the journal is up to date, and remove the
	 * journal file if it isn't, as we wouldn't be able to apply
	 * updates otherwise.
	 */
	if (zone->journal != NULL && dns_zone_isdynamic(zone, true) &&
	    ! DNS_ZONE_OPTION(zone, DNS_ZONEOPT_IXFRFROMDIFFS))
	{
		uint32_t jserial;
		dns_journal_t *journal = NULL;
		bool empty = false;

		result = dns_journal_open(zone->mctx, zone->journal,
					  DNS_JOURNAL_READ, &journal);
		if (result == ISC_R_SUCCESS) {
			jserial = dns_journal_last_serial(journal);
			empty = dns_journal_empty(journal);
			dns_journal_destroy(&journal);
		} else {
			jserial = serial;
			result = ISC_R_SUCCESS;
		}

		if (jserial != serial) {
			if (!empty) {
				dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
					      ISC_LOG_INFO,
					     "journal file is out of date: "
					     "removing journal file");
			}
			if (remove(zone->journal) < 0 && errno != ENOENT) {
				char strbuf[ISC_STRERRORSIZE];
				strerror_r(errno, strbuf, sizeof(strbuf));
				isc_log_write(dns_lctx,
					      DNS_LOGCATEGORY_GENERAL,
					      DNS_LOGMODULE_ZONE,
					      ISC_LOG_WARNING,
					      "unable to remove journal "
					      "'%s': '%s'",
					      zone->journal, strbuf);
			}
		}
	}

	dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD, ISC_LOG_DEBUG(1),
		      "loaded; checking validity");

	/*
	 * Master / Slave / Mirror / Stub zones require both NS and SOA records
	 * at the top of the zone.
	 */

	switch (zone->type) {
	case dns_zone_dlz:
	case dns_zone_master:
	case dns_zone_slave:
	case dns_zone_mirror:
	case dns_zone_stub:
	case dns_zone_redirect:
		if (soacount != 1) {
			dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
				      ISC_LOG_ERROR,
				     "has %d SOA records", soacount);
			result = DNS_R_BADZONE;
		}
		if (nscount == 0) {
			dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
				      ISC_LOG_ERROR,
				     "has no NS records");
			result = DNS_R_BADZONE;
		}
		if (result != ISC_R_SUCCESS) {
			goto cleanup;
		}
		if (zone->type == dns_zone_master && errors != 0) {
			result = DNS_R_BADZONE;
			goto cleanup;
		}
		if (zone->type != dns_zone_stub &&
		    zone->type != dns_zone_redirect)
		{
			result = check_nsec3param(zone, db);
			if (result != ISC_R_SUCCESS)
				goto cleanup;
		}
		if (zone->type == dns_zone_master &&
		    DNS_ZONE_OPTION(zone, DNS_ZONEOPT_CHECKINTEGRITY) &&
		    !integrity_checks(zone, db))
		{
			result = DNS_R_BADZONE;
			goto cleanup;
		}
		if (zone->type == dns_zone_master &&
		    DNS_ZONE_OPTION(zone, DNS_ZONEOPT_CHECKDUPRR) &&
		    !zone_check_dup(zone, db))
		{
			result = DNS_R_BADZONE;
			goto cleanup;
		}

		result = dns_zone_verifydb(zone, db, NULL);
		if (result != ISC_R_SUCCESS) {
			goto cleanup;
		}

		if (zone->db != NULL) {
			unsigned int oldsoacount;

			/*
			 * This is checked in zone_replacedb() for slave zones
			 * as they don't reload from disk.
			 */
			result = zone_get_from_db(zone, zone->db, NULL,
						  &oldsoacount, &oldserial,
						  NULL, NULL, NULL, NULL,
						  NULL);
			RUNTIME_CHECK(result == ISC_R_SUCCESS);
			RUNTIME_CHECK(soacount > 0U);
			if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_IXFRFROMDIFFS) &&
			    !isc_serial_gt(serial, oldserial)) {
				uint32_t serialmin, serialmax;

				INSIST(zone->type == dns_zone_master);
				INSIST(zone->raw == NULL);

				if (serial == oldserial &&
				    zone_unchanged(zone->db, db, zone->mctx)) {
					dns_zone_logc(zone,
						      DNS_LOGCATEGORY_ZONELOAD,
						      ISC_LOG_INFO,
						     "ixfr-from-differences: "
						     "unchanged");
					goto done;
				}

				serialmin = (oldserial + 1) & 0xffffffffU;
				serialmax = (oldserial + 0x7fffffffU) &
					     0xffffffffU;
				dns_zone_logc(zone,
					      DNS_LOGCATEGORY_ZONELOAD,
					      ISC_LOG_ERROR,
					      "ixfr-from-differences: "
					      "new serial (%u) out of range "
					      "[%u - %u]", serial, serialmin,
					      serialmax);
				result = DNS_R_BADZONE;
				goto cleanup;
			} else if (!isc_serial_ge(serial, oldserial)) {
				dns_zone_logc(zone,
					      DNS_LOGCATEGORY_ZONELOAD,
					      ISC_LOG_ERROR,
					      "zone serial (%u/%u) has gone "
					      "backwards", serial, oldserial);
			} else if (serial == oldserial && !hasinclude &&
				   strcmp(zone->db_argv[0], "_builtin") != 0)
			{
				dns_zone_logc(zone,
					      DNS_LOGCATEGORY_ZONELOAD,
					      ISC_LOG_ERROR,
					      "zone serial (%u) unchanged. "
					      "zone may fail to transfer "
					      "to slaves.", serial);
			}
		}

		if (zone->type == dns_zone_master &&
		    (zone->update_acl != NULL || zone->ssutable != NULL) &&
		    zone->sigresigninginterval < (3 * refresh) &&
		    dns_db_issecure(db))
		{
			dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
				      ISC_LOG_WARNING,
				      "sig-re-signing-interval less than "
				      "3 * refresh.");
		}

		zone->refresh = RANGE(refresh,
				      zone->minrefresh, zone->maxrefresh);
		zone->retry = RANGE(retry,
				    zone->minretry, zone->maxretry);
		zone->expire = RANGE(expire, zone->refresh + zone->retry,
				     DNS_MAX_EXPIRE);
		zone->minimum = minimum;
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_HAVETIMERS);

		if (zone->type == dns_zone_slave ||
		    zone->type == dns_zone_mirror ||
		    zone->type == dns_zone_stub ||
		    (zone->type == dns_zone_redirect &&
		     zone->masters != NULL))
		{
			isc_time_t t;
			uint32_t delay;

			result = isc_file_getmodtime(zone->journal, &t);
			if (result != ISC_R_SUCCESS) {
				result = isc_file_getmodtime(zone->masterfile,
							     &t);
			}
			if (result == ISC_R_SUCCESS) {
				DNS_ZONE_TIME_ADD(&t, zone->expire,
						  &zone->expiretime);
			} else {
				DNS_ZONE_TIME_ADD(&now, zone->retry,
						  &zone->expiretime);
			}

			delay = (zone->retry -
				 isc_random_uniform((zone->retry * 3) / 4));
			DNS_ZONE_TIME_ADD(&now, delay, &zone->refreshtime);
			if (isc_time_compare(&zone->refreshtime,
					     &zone->expiretime) >= 0)
			{
				zone->refreshtime = now;
			}
		}

		break;

	case dns_zone_key:
		result = sync_keyzone(zone, db);
		if (result != ISC_R_SUCCESS) {
			goto cleanup;
		}
		break;

	default:
		UNEXPECTED_ERROR(__FILE__, __LINE__,
				 "unexpected zone type %d", zone->type);
		result = ISC_R_UNEXPECTED;
		goto cleanup;
	}

	/*
	 * Check for weak DNSKEY's.
	 */
	if (zone->type == dns_zone_master) {
		zone_check_dnskeys(zone, db);
	}

	/*
	 * Schedule DNSSEC key refresh.
	 */
	if (zone->type == dns_zone_master &&
	    DNS_ZONEKEY_OPTION(zone, DNS_ZONEKEY_MAINTAIN))
	{
		zone->refreshkeytime = now;
	}

	ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_write);
	if (zone->db != NULL) {
		had_db = true;
		result = zone_replacedb(zone, db, false);
		ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_write);
		if (result != ISC_R_SUCCESS) {
			goto cleanup;
		}
	} else {
		zone_attachdb(zone, db);
		ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_write);
		DNS_ZONE_SETFLAG(zone,
				 DNS_ZONEFLG_LOADED|
				 DNS_ZONEFLG_NEEDSTARTUPNOTIFY);
		if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_SENDSECURE) &&
		    inline_raw(zone))
		{
			if (zone->secure->db == NULL) {
				zone_send_securedb(zone, db);
			} else {
				zone_send_secureserial(zone, serial);
			}
		}
	}

	/*
	 * Finished loading inline-signing zone; need to get status
	 * from the raw side now.
	 */
	if (zone->type == dns_zone_master && inline_secure(zone)) {
		maybe_send_secure(zone);
	}

	result = ISC_R_SUCCESS;

	if (needdump) {
		if (zone->type == dns_zone_key) {
			zone_needdump(zone, 30);
		} else {
			zone_needdump(zone, DNS_DUMP_DELAY);
		}
	}

	if (zone->task != NULL) {
		if (zone->type == dns_zone_master) {
			set_resigntime(zone);
			resume_signingwithkey(zone);
			resume_addnsec3chain(zone);
		}

		if (zone->type == dns_zone_master &&
		    !DNS_ZONEKEY_OPTION(zone, DNS_ZONEKEY_NORESIGN) &&
		    dns_zone_isdynamic(zone, false) &&
		    dns_db_issecure(db))
		{
			dns_name_t *name;
			dns_fixedname_t fixed;
			dns_rdataset_t next;

			dns_rdataset_init(&next);
			name = dns_fixedname_initname(&fixed);

			result = dns_db_getsigningtime(db, &next, name);
			if (result == ISC_R_SUCCESS) {
				isc_stdtime_t timenow;
				char namebuf[DNS_NAME_FORMATSIZE];
				char typebuf[DNS_RDATATYPE_FORMATSIZE];

				isc_stdtime_get(&timenow);
				dns_name_format(name, namebuf, sizeof(namebuf));
				dns_rdatatype_format(next.covers,
						     typebuf, sizeof(typebuf));
				dnssec_log(zone, ISC_LOG_DEBUG(3),
					   "next resign: %s/%s "
					   "in %d seconds", namebuf, typebuf,
					   next.resign - timenow -
					    zone->sigresigninginterval);
				dns_rdataset_disassociate(&next);
			} else {
				dnssec_log(zone, ISC_LOG_WARNING,
					   "signed dynamic zone has no "
					   "resign event scheduled");
			}
		}

		zone_settimer(zone, &now);
	}

	/*
	 * Clear old include list.
	 */
	for (inc = ISC_LIST_HEAD(zone->includes);
	     inc != NULL;
	     inc = ISC_LIST_HEAD(zone->includes))
	{
		ISC_LIST_UNLINK(zone->includes, inc, link);
		isc_mem_free(zone->mctx, inc->name);
		isc_mem_put(zone->mctx, inc, sizeof(*inc));
	}
	zone->nincludes = 0;

	/*
	 * Transfer new include list.
	 */
	for (inc = ISC_LIST_HEAD(zone->newincludes);
	     inc != NULL;
	     inc = ISC_LIST_HEAD(zone->newincludes))
	{
		ISC_LIST_UNLINK(zone->newincludes, inc, link);
		ISC_LIST_APPEND(zone->includes, inc, link);
		zone->nincludes++;
	}

	if (! dns_db_ispersistent(db)) {
		dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
			      ISC_LOG_INFO, "loaded serial %u%s", serial,
			      dns_db_issecure(db) ? " (DNSSEC signed)" : "");
	}

	if (!had_db && zone->type == dns_zone_mirror) {
		dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD, ISC_LOG_INFO,
			      "mirror zone is now in use");
	}

	zone->loadtime = loadtime;
	goto done;

 cleanup:
	if (zone->type == dns_zone_key && result != ISC_R_SUCCESS) {
		dnssec_log(zone, ISC_LOG_ERROR,
			   "failed to initialize managed-keys (%s): "
			   "DNSSEC validation is at risk",
			   isc_result_totext(result));
	}

	for (inc = ISC_LIST_HEAD(zone->newincludes);
	     inc != NULL;
	     inc = ISC_LIST_HEAD(zone->newincludes))
	{
		ISC_LIST_UNLINK(zone->newincludes, inc, link);
		isc_mem_free(zone->mctx, inc->name);
		isc_mem_put(zone->mctx, inc, sizeof(*inc));
	}
	if (zone->type == dns_zone_slave ||
	    zone->type == dns_zone_mirror ||
	    zone->type == dns_zone_stub ||
	    zone->type == dns_zone_key ||
	    (zone->type == dns_zone_redirect && zone->masters != NULL))
	{
		if (result != ISC_R_NOMEMORY) {
			if (zone->journal != NULL) {
				zone_saveunique(zone, zone->journal,
						"jn-XXXXXXXX");
			}
			if (zone->masterfile != NULL) {
				zone_saveunique(zone, zone->masterfile,
						"db-XXXXXXXX");
			}
		}

		/* Mark the zone for immediate refresh. */
		zone->refreshtime = now;
		if (zone->task != NULL) {
			zone_settimer(zone, &now);
		}
		result = ISC_R_SUCCESS;
	} else if (zone->type == dns_zone_master ||
		   zone->type == dns_zone_redirect)
	{
		if (! (inline_secure(zone) && result == ISC_R_FILENOTFOUND)) {
			dns_zone_logc(zone, DNS_LOGCATEGORY_ZONELOAD,
				      ISC_LOG_ERROR,
				      "not loaded due to errors.");
		} else if (zone->type == dns_zone_master) {
			result = ISC_R_SUCCESS;
		}
	}

 done:
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_LOADPENDING);
	/*
	 * If this is an inline-signed zone and we were called for the raw
	 * zone, we need to clear DNS_ZONEFLG_LOADPENDING for the secure zone
	 * as well, but only if this is a reload, not an initial zone load: in
	 * the former case, zone_postload() will not be run for the secure
	 * zone; in the latter case, it will be.  Check which case we are
	 * dealing with by consulting the DNS_ZONEFLG_LOADED flag for the
	 * secure zone: if it is set, this must be a reload.
	 */
	if (inline_raw(zone) &&
	    DNS_ZONE_FLAG(zone->secure, DNS_ZONEFLG_LOADED))
	{
		DNS_ZONE_CLRFLAG(zone->secure, DNS_ZONEFLG_LOADPENDING);
	}

	zone_debuglog(zone, "zone_postload", 99, "done");

	return (result);
}