dns_zone_checknames(dns_zone_t *zone, const dns_name_t *name,
		    dns_rdata_t *rdata)
{
	bool ok = true;
	bool fail = false;
	char namebuf[DNS_NAME_FORMATSIZE];
	char namebuf2[DNS_NAME_FORMATSIZE];
	char typebuf[DNS_RDATATYPE_FORMATSIZE];
	int level = ISC_LOG_WARNING;
	dns_name_t bad;

	REQUIRE(DNS_ZONE_VALID(zone));

	if (!DNS_ZONE_OPTION(zone, DNS_ZONEOPT_CHECKNAMES) &&
	    rdata->type != dns_rdatatype_nsec3)
		return (ISC_R_SUCCESS);

	if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_CHECKNAMESFAIL) ||
	    rdata->type == dns_rdatatype_nsec3) {
		level = ISC_LOG_ERROR;
		fail = true;
	}

	ok = dns_rdata_checkowner(name, rdata->rdclass, rdata->type, true);
	if (!ok) {
		dns_name_format(name, namebuf, sizeof(namebuf));
		dns_rdatatype_format(rdata->type, typebuf, sizeof(typebuf));
		dns_zone_log(zone, level, "%s/%s: %s", namebuf, typebuf,
			     dns_result_totext(DNS_R_BADOWNERNAME));
		if (fail)
			return (DNS_R_BADOWNERNAME);
	}

	dns_name_init(&bad, NULL);
	ok = dns_rdata_checknames(rdata, name, &bad);
	if (!ok) {
		dns_name_format(name, namebuf, sizeof(namebuf));
		dns_name_format(&bad, namebuf2, sizeof(namebuf2));
		dns_rdatatype_format(rdata->type, typebuf, sizeof(typebuf));
		dns_zone_log(zone, level, "%s/%s: %s: %s ", namebuf, typebuf,
			     namebuf2, dns_result_totext(DNS_R_BADNAME));
		if (fail)
			return (DNS_R_BADNAME);
	}

	return (ISC_R_SUCCESS);
}