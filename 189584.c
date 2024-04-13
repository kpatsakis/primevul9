process_opt(ns_client_t *client, dns_rdataset_t *opt) {
	dns_rdata_t rdata;
	isc_buffer_t optbuf;
	isc_result_t result;
	uint16_t optcode;
	uint16_t optlen;

	/*
	 * Set the client's UDP buffer size.
	 */
	client->udpsize = opt->rdclass;

	/*
	 * If the requested UDP buffer size is less than 512,
	 * ignore it and use 512.
	 */
	if (client->udpsize < 512)
		client->udpsize = 512;

	/*
	 * Get the flags out of the OPT record.
	 */
	client->extflags = (uint16_t)(opt->ttl & 0xFFFF);

	/*
	 * Do we understand this version of EDNS?
	 *
	 * XXXRTH need library support for this!
	 */
	client->ednsversion = (opt->ttl & 0x00FF0000) >> 16;
	if (client->ednsversion > DNS_EDNS_VERSION) {
		isc_stats_increment(ns_g_server->nsstats,
				    dns_nsstatscounter_badednsver);
		result = ns_client_addopt(client, client->message,
					  &client->opt);
		if (result == ISC_R_SUCCESS)
			result = DNS_R_BADVERS;
		ns_client_error(client, result);
		goto cleanup;
	}

	/* Check for NSID request */
	result = dns_rdataset_first(opt);
	if (result == ISC_R_SUCCESS) {
		dns_rdata_init(&rdata);
		dns_rdataset_current(opt, &rdata);
		isc_buffer_init(&optbuf, rdata.data, rdata.length);
		isc_buffer_add(&optbuf, rdata.length);
		while (isc_buffer_remaininglength(&optbuf) >= 4) {
			optcode = isc_buffer_getuint16(&optbuf);
			optlen = isc_buffer_getuint16(&optbuf);
			switch (optcode) {
			case DNS_OPT_NSID:
				if (!WANTNSID(client))
					isc_stats_increment(
						    ns_g_server->nsstats,
						    dns_nsstatscounter_nsidopt);
				client->attributes |= NS_CLIENTATTR_WANTNSID;
				isc_buffer_forward(&optbuf, optlen);
				break;
			case DNS_OPT_COOKIE:
				process_cookie(client, &optbuf, optlen);
				break;
			case DNS_OPT_EXPIRE:
				if (!WANTEXPIRE(client))
					isc_stats_increment(
						  ns_g_server->nsstats,
						  dns_nsstatscounter_expireopt);
				client->attributes |= NS_CLIENTATTR_WANTEXPIRE;
				isc_buffer_forward(&optbuf, optlen);
				break;
			case DNS_OPT_CLIENT_SUBNET:
				result = process_ecs(client, &optbuf, optlen);
				if (result != ISC_R_SUCCESS) {
					ns_client_error(client, result);
					goto cleanup;
				}
				isc_stats_increment(ns_g_server->nsstats,
						  dns_nsstatscounter_ecsopt);
				break;
			case DNS_OPT_KEY_TAG:
				result = process_keytag(client, &optbuf,
							optlen);
				if (result != ISC_R_SUCCESS) {
					ns_client_error(client, result);
					return (result);
				}
				isc_stats_increment(ns_g_server->nsstats,
						   dns_nsstatscounter_keytagopt);
				break;
			default:
				isc_stats_increment(ns_g_server->nsstats,
						  dns_nsstatscounter_otheropt);
				isc_buffer_forward(&optbuf, optlen);
				break;
			}
		}
	}

	isc_stats_increment(ns_g_server->nsstats, dns_nsstatscounter_edns0in);
	client->attributes |= NS_CLIENTATTR_WANTOPT;

 cleanup:
	return (result);
}