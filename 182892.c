dns_zone_notifyreceive(dns_zone_t *zone, isc_sockaddr_t *from,
		       isc_sockaddr_t *to, dns_message_t *msg)
{
	unsigned int i;
	dns_rdata_soa_t soa;
	dns_rdataset_t *rdataset = NULL;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	isc_result_t result;
	char fromtext[ISC_SOCKADDR_FORMATSIZE];
	int match = 0;
	isc_netaddr_t netaddr;
	uint32_t serial = 0;
	bool have_serial = false;
	dns_tsigkey_t *tsigkey;
	dns_name_t *tsig;

	REQUIRE(DNS_ZONE_VALID(zone));

	/*
	 * If type != T_SOA return DNS_R_NOTIMP.  We don't yet support
	 * ROLLOVER.
	 *
	 * SOA:	RFC1996
	 * Check that 'from' is a valid notify source, (zone->masters).
	 *	Return DNS_R_REFUSED if not.
	 *
	 * If the notify message contains a serial number check it
	 * against the zones serial and return if <= current serial
	 *
	 * If a refresh check is progress, if so just record the
	 * fact we received a NOTIFY and from where and return.
	 * We will perform a new refresh check when the current one
	 * completes. Return ISC_R_SUCCESS.
	 *
	 * Otherwise initiate a refresh check using 'from' as the
	 * first address to check.  Return ISC_R_SUCCESS.
	 */

	isc_sockaddr_format(from, fromtext, sizeof(fromtext));

	/*
	 * Notify messages are processed by the raw zone.
	 */
	LOCK_ZONE(zone);
	INSIST(zone != zone->raw);
	if (inline_secure(zone)) {
		result = dns_zone_notifyreceive(zone->raw, from, to, msg);
		UNLOCK_ZONE(zone);
		return (result);
	}
	/*
	 *  We only handle NOTIFY (SOA) at the present.
	 */
	if (isc_sockaddr_pf(from) == PF_INET)
		inc_stats(zone, dns_zonestatscounter_notifyinv4);
	else
		inc_stats(zone, dns_zonestatscounter_notifyinv6);
	if (msg->counts[DNS_SECTION_QUESTION] == 0 ||
	    dns_message_findname(msg, DNS_SECTION_QUESTION, &zone->origin,
				 dns_rdatatype_soa, dns_rdatatype_none,
				 NULL, NULL) != ISC_R_SUCCESS) {
		UNLOCK_ZONE(zone);
		if (msg->counts[DNS_SECTION_QUESTION] == 0) {
			dns_zone_log(zone, ISC_LOG_NOTICE,
				     "NOTIFY with no "
				     "question section from: %s", fromtext);
			return (DNS_R_FORMERR);
		}
		dns_zone_log(zone, ISC_LOG_NOTICE,
			     "NOTIFY zone does not match");
		return (DNS_R_NOTIMP);
	}

	/*
	 * If we are a master zone just succeed.
	 */
	if (zone->type == dns_zone_master) {
		UNLOCK_ZONE(zone);
		return (ISC_R_SUCCESS);
	}

	isc_netaddr_fromsockaddr(&netaddr, from);
	for (i = 0; i < zone->masterscnt; i++) {
		if (isc_sockaddr_eqaddr(from, &zone->masters[i]))
			break;
		if (zone->view->aclenv.match_mapped &&
		    IN6_IS_ADDR_V4MAPPED(&from->type.sin6.sin6_addr) &&
		    isc_sockaddr_pf(&zone->masters[i]) == AF_INET) {
			isc_netaddr_t na1, na2;
			isc_netaddr_fromv4mapped(&na1, &netaddr);
			isc_netaddr_fromsockaddr(&na2, &zone->masters[i]);
			if (isc_netaddr_equal(&na1, &na2))
				break;
		}
	}

	/*
	 * Accept notify requests from non masters if they are on
	 * 'zone->notify_acl'.
	 */
	tsigkey = dns_message_gettsigkey(msg);
	tsig = dns_tsigkey_identity(tsigkey);
	if (i >= zone->masterscnt && zone->notify_acl != NULL &&
	    (dns_acl_match(&netaddr, tsig, zone->notify_acl,
			   &zone->view->aclenv, &match,
			   NULL) == ISC_R_SUCCESS) &&
	    match > 0)
	{
		/* Accept notify. */
	} else if (i >= zone->masterscnt) {
		UNLOCK_ZONE(zone);
		dns_zone_log(zone, ISC_LOG_INFO,
			     "refused notify from non-master: %s", fromtext);
		inc_stats(zone, dns_zonestatscounter_notifyrej);
		return (DNS_R_REFUSED);
	}

	/*
	 * If the zone is loaded and there are answers check the serial
	 * to see if we need to do a refresh.  Do not worry about this
	 * check if we are a dialup zone as we use the notify request
	 * to trigger a refresh check.
	 */
	if (msg->counts[DNS_SECTION_ANSWER] > 0 &&
	    DNS_ZONE_FLAG(zone, DNS_ZONEFLG_LOADED) &&
	    !DNS_ZONE_FLAG(zone, DNS_ZONEFLG_NOREFRESH)) {
		result = dns_message_findname(msg, DNS_SECTION_ANSWER,
					      &zone->origin,
					      dns_rdatatype_soa,
					      dns_rdatatype_none, NULL,
					      &rdataset);
		if (result == ISC_R_SUCCESS)
			result = dns_rdataset_first(rdataset);
		if (result == ISC_R_SUCCESS) {
			uint32_t oldserial;
			unsigned int soacount;

			dns_rdataset_current(rdataset, &rdata);
			result = dns_rdata_tostruct(&rdata, &soa, NULL);
			RUNTIME_CHECK(result == ISC_R_SUCCESS);
			serial = soa.serial;
			have_serial = true;
			/*
			 * The following should safely be performed without DB
			 * lock and succeed in this context.
			 */
			result = zone_get_from_db(zone, zone->db, NULL,
						  &soacount, &oldserial, NULL,
						  NULL, NULL, NULL, NULL);
			RUNTIME_CHECK(result == ISC_R_SUCCESS);
			RUNTIME_CHECK(soacount > 0U);
			if (isc_serial_le(serial, oldserial)) {
				dns_zone_log(zone,
					     ISC_LOG_INFO,
					     "notify from %s: "
					     "zone is up to date",
					     fromtext);
				UNLOCK_ZONE(zone);
				return (ISC_R_SUCCESS);
			}
		}
	}

	/*
	 * If we got this far and there was a refresh in progress just
	 * let it complete.  Record where we got the notify from so we
	 * can perform a refresh check when the current one completes
	 */
	if (DNS_ZONE_FLAG(zone, DNS_ZONEFLG_REFRESH)) {
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NEEDREFRESH);
		zone->notifyfrom = *from;
		UNLOCK_ZONE(zone);
		if (have_serial)
			dns_zone_log(zone, ISC_LOG_INFO,
				     "notify from %s: serial %u: refresh in "
				     "progress, refresh check queued",
				      fromtext, serial);
		else
			dns_zone_log(zone, ISC_LOG_INFO,
				     "notify from %s: refresh in progress, "
				     "refresh check queued", fromtext);
		return (ISC_R_SUCCESS);
	}
	if (have_serial)
		dns_zone_log(zone, ISC_LOG_INFO, "notify from %s: serial %u",
			     fromtext, serial);
	else
		dns_zone_log(zone, ISC_LOG_INFO, "notify from %s: no serial",
			     fromtext);
	zone->notifyfrom = *from;
	UNLOCK_ZONE(zone);

	if (to != NULL) {
		dns_zonemgr_unreachabledel(zone->zmgr, from, to);
	}
	dns_zone_refresh(zone);
	return (ISC_R_SUCCESS);
}