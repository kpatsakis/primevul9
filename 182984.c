zone_xfrdone(dns_zone_t *zone, isc_result_t result) {
	isc_time_t now;
	bool again = false;
	unsigned int soacount;
	unsigned int nscount;
	uint32_t serial, refresh, retry, expire, minimum;
	isc_result_t xfrresult = result;
	bool free_needed;
	dns_zone_t *secure = NULL;

	REQUIRE(DNS_ZONE_VALID(zone));

	dns_zone_log(zone, ISC_LOG_DEBUG(1),
		     "zone transfer finished: %s", dns_result_totext(result));

	/*
	 * Obtaining a lock on the zone->secure (see zone_send_secureserial)
	 * could result in a deadlock due to a LOR so we will spin if we
	 * can't obtain the both locks.
	 */
 again:
	LOCK_ZONE(zone);
	if (inline_raw(zone)) {
		secure = zone->secure;
		INSIST(secure != zone);
		TRYLOCK_ZONE(result, secure);
		if (result != ISC_R_SUCCESS) {
			UNLOCK_ZONE(zone);
			secure = NULL;
			isc_thread_yield();
			goto again;
		}
	}

	INSIST((zone->flags & DNS_ZONEFLG_REFRESH) != 0);
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_REFRESH);
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_SOABEFOREAXFR);

	TIME_NOW(&now);
	switch (xfrresult) {
	case ISC_R_SUCCESS:
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NEEDNOTIFY);
		/* FALLTHROUGH */
	case DNS_R_UPTODATE:
		DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_FORCEXFER);
		/*
		 * Has the zone expired underneath us?
		 */
		ZONEDB_LOCK(&zone->dblock, isc_rwlocktype_read);
		if (zone->db == NULL) {
			ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
			goto same_master;
		}

		/*
		 * Update the zone structure's data from the actual
		 * SOA received.
		 */
		nscount = 0;
		soacount = 0;
		INSIST(zone->db != NULL);
		result = zone_get_from_db(zone, zone->db, &nscount,
					  &soacount, &serial, &refresh,
					  &retry, &expire, &minimum, NULL);
		ZONEDB_UNLOCK(&zone->dblock, isc_rwlocktype_read);
		if (result == ISC_R_SUCCESS) {
			if (soacount != 1)
				dns_zone_log(zone, ISC_LOG_ERROR,
					     "transferred zone "
					     "has %d SOA record%s", soacount,
					     (soacount != 0) ? "s" : "");
			if (nscount == 0) {
				dns_zone_log(zone, ISC_LOG_ERROR,
					     "transferred zone "
					     "has no NS records");
				if (DNS_ZONE_FLAG(zone,
						  DNS_ZONEFLG_HAVETIMERS)) {
					zone->refresh = DNS_ZONE_DEFAULTREFRESH;
					zone->retry = DNS_ZONE_DEFAULTRETRY;
				}
				DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_HAVETIMERS);
				zone_unload(zone);
				goto next_master;
			}
			zone->refresh = RANGE(refresh, zone->minrefresh,
					      zone->maxrefresh);
			zone->retry = RANGE(retry, zone->minretry,
					    zone->maxretry);
			zone->expire = RANGE(expire,
					     zone->refresh + zone->retry,
					     DNS_MAX_EXPIRE);
			zone->minimum = minimum;
			DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_HAVETIMERS);
		}

		/*
		 * Set our next update/expire times.
		 */
		if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NEEDREFRESH)) {
			DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_NEEDREFRESH);
			zone->refreshtime = now;
			DNS_ZONE_TIME_ADD(&now, zone->expire,
					  &zone->expiretime);
		} else {
			DNS_ZONE_JITTER_ADD(&now, zone->refresh,
					    &zone->refreshtime);
			DNS_ZONE_TIME_ADD(&now, zone->expire,
					  &zone->expiretime);
		}
		if (result == ISC_R_SUCCESS && xfrresult == ISC_R_SUCCESS) {
			char buf[DNS_NAME_FORMATSIZE + sizeof(": TSIG ''")];
			if (zone->tsigkey != NULL) {
				char namebuf[DNS_NAME_FORMATSIZE];
				dns_name_format(&zone->tsigkey->name, namebuf,
						sizeof(namebuf));
				snprintf(buf, sizeof(buf), ": TSIG '%s'",
					 namebuf);
			} else
				buf[0] = '\0';
			dns_zone_log(zone, ISC_LOG_INFO,
				     "transferred serial %u%s",
				     serial, buf);
			if (inline_raw(zone))
				zone_send_secureserial(zone, serial);
		}

		/*
		 * This is not necessary if we just performed a AXFR
		 * however it is necessary for an IXFR / UPTODATE and
		 * won't hurt with an AXFR.
		 */
		if (zone->masterfile != NULL || zone->journal != NULL) {
			unsigned int delay = DNS_DUMP_DELAY;

			result = ISC_R_FAILURE;
			if (zone->journal != NULL)
				result = isc_file_settime(zone->journal, &now);
			if (result != ISC_R_SUCCESS &&
			    zone->masterfile != NULL)
				result = isc_file_settime(zone->masterfile,
							  &now);

			if ((DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NODELAY) != 0) ||
			    result == ISC_R_FILENOTFOUND)
				delay = 0;

			if ((result == ISC_R_SUCCESS ||
			    result == ISC_R_FILENOTFOUND) &&
			    zone->masterfile != NULL)
				zone_needdump(zone, delay);
			else if (result != ISC_R_SUCCESS)
				dns_zone_log(zone, ISC_LOG_ERROR,
					     "transfer: could not set file "
					     "modification time of '%s': %s",
					     zone->masterfile,
					     dns_result_totext(result));
		}
		DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_NODELAY);
		inc_stats(zone, dns_zonestatscounter_xfrsuccess);
		break;

	case DNS_R_BADIXFR:
		/* Force retry with AXFR. */
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLAG_NOIXFR);
		goto same_master;

	case DNS_R_TOOMANYRECORDS:
	case DNS_R_VERIFYFAILURE:
		DNS_ZONE_JITTER_ADD(&now, zone->refresh, &zone->refreshtime);
		inc_stats(zone, dns_zonestatscounter_xfrfail);
		break;

	default:
	next_master:
		/*
		 * Skip to next failed / untried master.
		 */
		do {
			zone->curmaster++;
		} while (zone->curmaster < zone->masterscnt &&
			 zone->mastersok[zone->curmaster]);
		/* FALLTHROUGH */
	same_master:
		if (zone->curmaster >= zone->masterscnt) {
			zone->curmaster = 0;
			if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_USEALTXFRSRC) &&
			    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_USEALTXFRSRC)) {
				DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_REFRESH);
				DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_USEALTXFRSRC);
				while (zone->curmaster < zone->masterscnt &&
				       zone->mastersok[zone->curmaster])
					zone->curmaster++;
				again = true;
			} else
				DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_USEALTXFRSRC);
		} else {
			DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_REFRESH);
			again = true;
		}
		inc_stats(zone, dns_zonestatscounter_xfrfail);
		break;
	}
	zone_settimer(zone, &now);

	/*
	 * If creating the transfer object failed, zone->xfr is NULL.
	 * Otherwise, we are called as the done callback of a zone
	 * transfer object that just entered its shutting-down
	 * state.  Since we are no longer responsible for shutting
	 * it down, we can detach our reference.
	 */
	if (zone->xfr != NULL)
		dns_xfrin_detach(&zone->xfr);

	if (zone->tsigkey != NULL)
		dns_tsigkey_detach(&zone->tsigkey);

	/*
	 * Handle any deferred journal compaction.
	 */
	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NEEDCOMPACT)) {
		dns_db_t *db = NULL;
		if (dns_zone_getdb(zone, &db) == ISC_R_SUCCESS) {
			zone_journal_compact(zone, db, zone->compact_serial);
			dns_db_detach(&db);
			DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_NEEDCOMPACT);
		}
	}

	if (secure != NULL)
		UNLOCK_ZONE(secure);
	/*
	 * This transfer finishing freed up a transfer quota slot.
	 * Let any other zones waiting for quota have it.
	 */
	if (zone->zmgr != NULL &&
	    zone->statelist == &zone->zmgr->xfrin_in_progress) {
		UNLOCK_ZONE(zone);
		RWLOCK(&zone->zmgr->rwlock, isc_rwlocktype_write);
		ISC_LIST_UNLINK(zone->zmgr->xfrin_in_progress, zone, statelink);
		zone->statelist = NULL;
		zmgr_resume_xfrs(zone->zmgr, false);
		RWUNLOCK(&zone->zmgr->rwlock, isc_rwlocktype_write);
		LOCK_ZONE(zone);
	}

	/*
	 * Retry with a different server if necessary.
	 */
	if (again && !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_EXITING))
		queue_soa_query(zone);

	INSIST(zone->irefs > 0);
	zone->irefs--;
	free_needed = exit_check(zone);
	UNLOCK_ZONE(zone);
	if (free_needed)
		zone_free(zone);
}