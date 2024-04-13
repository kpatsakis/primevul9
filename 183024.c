add_opt(dns_message_t *message, uint16_t udpsize, bool reqnsid,
	bool reqexpire)
{
	isc_result_t result;
	dns_rdataset_t *rdataset = NULL;
	dns_ednsopt_t ednsopts[DNS_EDNSOPTIONS];
	int count = 0;

	/* Set EDNS options if applicable */
	if (reqnsid) {
		INSIST(count < DNS_EDNSOPTIONS);
		ednsopts[count].code = DNS_OPT_NSID;
		ednsopts[count].length = 0;
		ednsopts[count].value = NULL;
		count++;
	}
	if (reqexpire) {
		INSIST(count < DNS_EDNSOPTIONS);
		ednsopts[count].code = DNS_OPT_EXPIRE;
		ednsopts[count].length = 0;
		ednsopts[count].value = NULL;
		count++;
	}
	result = dns_message_buildopt(message, &rdataset, 0, udpsize, 0,
				      ednsopts, count);
	if (result != ISC_R_SUCCESS)
		return (result);

	return (dns_message_setopt(message, rdataset));
}