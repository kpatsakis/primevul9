get_edns_expire(dns_zone_t * zone, dns_message_t *message,
		uint32_t *expirep)
{
	isc_result_t result;
	uint32_t expire;
	dns_rdata_t rdata = DNS_RDATA_INIT;
	isc_buffer_t optbuf;
	uint16_t optcode;
	uint16_t optlen;

	REQUIRE(expirep != NULL);
	REQUIRE(message != NULL);

	if (message->opt == NULL)
		return;

	result = dns_rdataset_first(message->opt);
	if (result == ISC_R_SUCCESS) {
		dns_rdataset_current(message->opt, &rdata);
		isc_buffer_init(&optbuf, rdata.data, rdata.length);
		isc_buffer_add(&optbuf, rdata.length);
		while (isc_buffer_remaininglength(&optbuf) >= 4) {
			optcode = isc_buffer_getuint16(&optbuf);
			optlen = isc_buffer_getuint16(&optbuf);
			/*
			 * A EDNS EXPIRE response has a length of 4.
			 */
			if (optcode != DNS_OPT_EXPIRE || optlen != 4) {
				isc_buffer_forward(&optbuf, optlen);
				continue;
			}
			expire = isc_buffer_getuint32(&optbuf);
			dns_zone_log(zone, ISC_LOG_DEBUG(1),
				     "got EDNS EXPIRE of %u", expire);
			/*
			 * Trim *expirep?
			 */
			if (expire < *expirep)
				*expirep = expire;
			break;
		}
	}
}