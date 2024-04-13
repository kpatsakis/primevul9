process_cookie(ns_client_t *client, isc_buffer_t *buf, size_t optlen) {
	ns_altsecret_t *altsecret;
	unsigned char dbuf[COOKIE_SIZE];
	unsigned char *old;
	isc_stdtime_t now;
	uint32_t when;
	uint32_t nonce;
	isc_buffer_t db;

	/*
	 * If we have already seen a cookie option skip this cookie option.
	 */
	if ((!ns_g_server->answercookie) ||
	    (client->attributes & NS_CLIENTATTR_WANTCOOKIE) != 0)
	{
		isc_buffer_forward(buf, (unsigned int)optlen);
		return;
	}

	client->attributes |= NS_CLIENTATTR_WANTCOOKIE;

	isc_stats_increment(ns_g_server->nsstats, dns_nsstatscounter_cookiein);

	if (optlen != COOKIE_SIZE) {
		/*
		 * Not our token.
		 */
		INSIST(optlen >= 8U);
		memmove(client->cookie, isc_buffer_current(buf), 8);
		isc_buffer_forward(buf, (unsigned int)optlen);

		if (optlen == 8U)
			isc_stats_increment(ns_g_server->nsstats,
					    dns_nsstatscounter_cookienew);
		else
			isc_stats_increment(ns_g_server->nsstats,
					    dns_nsstatscounter_cookiebadsize);
		return;
	}

	/*
	 * Process all of the incoming buffer.
	 */
	old = isc_buffer_current(buf);
	memmove(client->cookie, old, 8);
	isc_buffer_forward(buf, 8);
	nonce = isc_buffer_getuint32(buf);
	when = isc_buffer_getuint32(buf);
	isc_buffer_forward(buf, 8);

	/*
	 * Allow for a 5 minute clock skew between servers sharing a secret.
	 * Only accept COOKIE if we have talked to the client in the last hour.
	 */
	isc_stdtime_get(&now);
	if (isc_serial_gt(when, (now + 300)) ||		/* In the future. */
	    isc_serial_lt(when, (now - 3600))) {	/* In the past. */
		isc_stats_increment(ns_g_server->nsstats,
				    dns_nsstatscounter_cookiebadtime);
		return;
	}

	isc_buffer_init(&db, dbuf, sizeof(dbuf));
	compute_cookie(client, when, nonce, ns_g_server->secret, &db);

	if (isc_safe_memequal(old, dbuf, COOKIE_SIZE)) {
		isc_stats_increment(ns_g_server->nsstats,
				    dns_nsstatscounter_cookiematch);
		client->attributes |= NS_CLIENTATTR_HAVECOOKIE;
		return;
	}

	for (altsecret = ISC_LIST_HEAD(ns_g_server->altsecrets);
	     altsecret != NULL;
	     altsecret = ISC_LIST_NEXT(altsecret, link))
	{
		isc_buffer_init(&db, dbuf, sizeof(dbuf));
		compute_cookie(client, when, nonce, altsecret->secret, &db);
		if (isc_safe_memequal(old, dbuf, COOKIE_SIZE)) {
			isc_stats_increment(ns_g_server->nsstats,
					    dns_nsstatscounter_cookiematch);
			client->attributes |= NS_CLIENTATTR_HAVECOOKIE;
			return;
		}
	}

	isc_stats_increment(ns_g_server->nsstats,
			    dns_nsstatscounter_cookienomatch);
}