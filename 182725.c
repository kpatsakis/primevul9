zone_settimer(dns_zone_t *zone, isc_time_t *now) {
	const char me[] = "zone_settimer";
	isc_time_t next;
	isc_result_t result;

	REQUIRE(DNS_ZONE_VALID(zone));
	ENTER;

	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_EXITING))
		return;

	isc_time_settoepoch(&next);

	switch (zone->type) {
	case dns_zone_redirect:
		if (zone->masters != NULL)
			goto treat_as_slave;
		/* FALLTHROUGH */

	case dns_zone_master:
		if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NEEDNOTIFY) ||
		    DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NEEDSTARTUPNOTIFY))
			next = zone->notifytime;
		if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NEEDDUMP) &&
		    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_DUMPING)) {
			INSIST(!isc_time_isepoch(&zone->dumptime));
			if (isc_time_isepoch(&next) ||
			    isc_time_compare(&zone->dumptime, &next) < 0)
				next = zone->dumptime;
		}
		if (zone->type == dns_zone_redirect)
			break;
		if (!DNS_ZONE_FLAG(zone, DNS_ZONEFLG_REFRESHING) &&
		    !isc_time_isepoch(&zone->refreshkeytime)) {
			if (isc_time_isepoch(&next) ||
			    isc_time_compare(&zone->refreshkeytime, &next) < 0)
				next = zone->refreshkeytime;
		}
		if (!isc_time_isepoch(&zone->resigntime)) {
			if (isc_time_isepoch(&next) ||
			    isc_time_compare(&zone->resigntime, &next) < 0)
				next = zone->resigntime;
		}
		if (!isc_time_isepoch(&zone->keywarntime)) {
			if (isc_time_isepoch(&next) ||
			    isc_time_compare(&zone->keywarntime, &next) < 0)
				next = zone->keywarntime;
		}
		if (!isc_time_isepoch(&zone->signingtime)) {
			if (isc_time_isepoch(&next) ||
			    isc_time_compare(&zone->signingtime, &next) < 0)
				next = zone->signingtime;
		}
		if (!isc_time_isepoch(&zone->nsec3chaintime)) {
			if (isc_time_isepoch(&next) ||
			    isc_time_compare(&zone->nsec3chaintime, &next) < 0)
				next = zone->nsec3chaintime;
		}
		break;

	case dns_zone_slave:
	case dns_zone_mirror:
	treat_as_slave:
		if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NEEDNOTIFY) ||
		    DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NEEDSTARTUPNOTIFY))
			next = zone->notifytime;
		/* FALLTHROUGH */

	case dns_zone_stub:
		if (!DNS_ZONE_FLAG(zone, DNS_ZONEFLG_REFRESH) &&
		    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NOMASTERS) &&
		    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NOREFRESH) &&
		    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADING) &&
		    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADPENDING) &&
		    !isc_time_isepoch(&zone->refreshtime) &&
		    (isc_time_isepoch(&next) ||
		     isc_time_compare(&zone->refreshtime, &next) < 0))
			next = zone->refreshtime;
		if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADED) &&
		    !isc_time_isepoch(&zone->expiretime)) {
			if (isc_time_isepoch(&next) ||
			     isc_time_compare(&zone->expiretime, &next) < 0)
				next = zone->expiretime;
		}
		if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NEEDDUMP) &&
		    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_DUMPING)) {
			INSIST(!isc_time_isepoch(&zone->dumptime));
			if (isc_time_isepoch(&next) ||
			    isc_time_compare(&zone->dumptime, &next) < 0)
				next = zone->dumptime;
		}
		break;

	case dns_zone_key:
		if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NEEDDUMP) &&
		    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_DUMPING)) {
			INSIST(!isc_time_isepoch(&zone->dumptime));
			if (isc_time_isepoch(&next) ||
			    isc_time_compare(&zone->dumptime, &next) < 0)
				next = zone->dumptime;
		}
		if (!DNS_ZONE_FLAG(zone, DNS_ZONEFLG_REFRESHING)) {
			if (isc_time_isepoch(&next) ||
			    (!isc_time_isepoch(&zone->refreshkeytime) &&
			    isc_time_compare(&zone->refreshkeytime, &next) < 0))
				next = zone->refreshkeytime;
		}
		break;

	default:
		break;
	}

	if (isc_time_isepoch(&next)) {
		zone_debuglog(zone, me, 10, "settimer inactive");
		result = isc_timer_reset(zone->timer, isc_timertype_inactive,
					  NULL, NULL, true);
		if (result != ISC_R_SUCCESS)
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "could not deactivate zone timer: %s",
				     isc_result_totext(result));
	} else {
		if (isc_time_compare(&next, now) <= 0)
			next = *now;
		result = isc_timer_reset(zone->timer, isc_timertype_once,
					 &next, NULL, true);
		if (result != ISC_R_SUCCESS)
			dns_zone_log(zone, ISC_LOG_ERROR,
				     "could not reset zone timer: %s",
				     isc_result_totext(result));
	}
}