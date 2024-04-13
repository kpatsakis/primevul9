zone_replacedb(dns_zone_t *zone, dns_db_t *db, bool dump) {
	dns_dbversion_t *ver;
	isc_result_t result;
	unsigned int soacount = 0;
	unsigned int nscount = 0;

	/*
	 * 'zone' and 'zone->db' locked by caller.
	 */
	REQUIRE(DNS_ZONE_VALID(zone));
	REQUIRE(LOCKED_ZONE(zone));
	if (inline_raw(zone))
		REQUIRE(LOCKED_ZONE(zone->secure));

	result = zone_get_from_db(zone, db, &nscount, &soacount,
				  NULL, NULL, NULL, NULL, NULL, NULL);
	if (result == ISC_R_SUCCESS) {
		if (soacount != 1) {
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "has %d SOA records", soacount);
			result = DNS_R_BADZONE;
		}
		if (nscount == 0 && zone->type != dns_zone_key) {
			dns_zone_log(zone, ISC_LOG_ERROR, "has no NS records");
			result = DNS_R_BADZONE;
		}
		if (result != ISC_R_SUCCESS)
			return (result);
	} else {
		dns_zone_log(zone, ISC_LOG_ERROR,
			    "retrieving SOA and NS records failed: %s",
			    dns_result_totext(result));
		return (result);
	}

	result = check_nsec3param(zone, db);
	if (result != ISC_R_SUCCESS)
		return (result);

	ver = NULL;
	dns_db_currentversion(db, &ver);

	/*
	 * The initial version of a slave zone is always dumped;
	 * subsequent versions may be journaled instead if this
	 * is enabled in the configuration.
	 */
	if (zone->db != NULL && zone->journal != NULL &&
	    DNS_ZONE_OPTION(zone, DNS_ZONEOPT_IXFRFROMDIFFS) &&
	    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_FORCEXFER))
	{
		uint32_t serial, oldserial;

		dns_zone_log(zone, ISC_LOG_DEBUG(3), "generating diffs");

		result = dns_db_getsoaserial(db, ver, &serial);
		if (result != ISC_R_SUCCESS) {
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "ixfr-from-differences: unable to get "
				     "new serial");
			goto fail;
		}

		/*
		 * This is checked in zone_postload() for master zones.
		 */
		result = zone_get_from_db(zone, zone->db, NULL, &soacount,
					  &oldserial, NULL, NULL, NULL, NULL,
					  NULL);
		RUNTIME_CHECK(result == ISC_R_SUCCESS);
		RUNTIME_CHECK(soacount > 0U);
		if ((zone->type == dns_zone_slave ||
		     (zone->type == dns_zone_redirect &&
		      zone->masters != NULL))
		    && !isc_serial_gt(serial, oldserial)) {
			uint32_t serialmin, serialmax;
			serialmin = (oldserial + 1) & 0xffffffffU;
			serialmax = (oldserial + 0x7fffffffU) & 0xffffffffU;
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "ixfr-from-differences: failed: "
				     "new serial (%u) out of range [%u - %u]",
				     serial, serialmin, serialmax);
			result = ISC_R_RANGE;
			goto fail;
		}

		result = dns_db_diff(zone->mctx, db, ver, zone->db, NULL,
				     zone->journal);
		if (result != ISC_R_SUCCESS) {
			char strbuf[ISC_STRERRORSIZE];
			strerror_r(errno, strbuf, sizeof(strbuf));
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "ixfr-from-differences: failed: "
				     "%s", strbuf);
			goto fallback;
		}
		if (dump)
			zone_needdump(zone, DNS_DUMP_DELAY);
		else
			zone_journal_compact(zone, zone->db, serial);
		if (zone->type == dns_zone_master && inline_raw(zone))
			zone_send_secureserial(zone, serial);
	} else {
 fallback:
		if (dump && zone->masterfile != NULL) {
			/*
			 * If DNS_ZONEFLG_FORCEXFER was set we don't want
			 * to keep the old masterfile.
			 */
			if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_FORCEXFER) &&
			    remove(zone->masterfile) < 0 && errno != ENOENT) {
				char strbuf[ISC_STRERRORSIZE];
				strerror_r(errno, strbuf, sizeof(strbuf));
				isc_log_write(dns_lctx,
					      DNS_LOGCATEGORY_GENERAL,
					      DNS_LOGMODULE_ZONE,
					      ISC_LOG_WARNING,
					      "unable to remove masterfile "
					      "'%s': '%s'",
					      zone->masterfile, strbuf);
			}
			if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADED) == 0)
				DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NODELAY);
			else
				zone_needdump(zone, 0);
		}
		if (dump && zone->journal != NULL) {
			/*
			 * The in-memory database just changed, and
			 * because 'dump' is set, it didn't change by
			 * being loaded from disk.  Also, we have not
			 * journaled diffs for this change.
			 * Therefore, the on-disk journal is missing
			 * the deltas for this change.	Since it can
			 * no longer be used to bring the zone
			 * up-to-date, it is useless and should be
			 * removed.
			 */
			isc_log_write(dns_lctx, DNS_LOGCATEGORY_GENERAL,
				      DNS_LOGMODULE_ZONE, ISC_LOG_DEBUG(3),
				      "removing journal file");
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

		if (inline_raw(zone))
			zone_send_securedb(zone, db);
	}

	dns_db_closeversion(db, &ver, false);

	dns_zone_log(zone, ISC_LOG_DEBUG(3), "replacing zone database");

	if (zone->db != NULL)
		zone_detachdb(zone);
	zone_attachdb(zone, db);
	dns_db_settask(zone->db, zone->task);
	DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_LOADED|DNS_ZONEFLG_NEEDNOTIFY);
	return (ISC_R_SUCCESS);

 fail:
	dns_db_closeversion(db, &ver, false);
	return (result);
}