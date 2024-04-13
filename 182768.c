zone_check_ns(dns_zone_t *zone, dns_db_t *db, dns_dbversion_t *version,
	      dns_name_t *name, bool logit)
{
	isc_result_t result;
	char namebuf[DNS_NAME_FORMATSIZE];
	char altbuf[DNS_NAME_FORMATSIZE];
	dns_fixedname_t fixed;
	dns_name_t *foundname;
	int level;

	if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_NOCHECKNS))
		return (true);

	if (zone->type == dns_zone_master)
		level = ISC_LOG_ERROR;
	else
		level = ISC_LOG_WARNING;

	foundname = dns_fixedname_initname(&fixed);

	result = dns_db_find(db, name, version, dns_rdatatype_a,
			     0, 0, NULL, foundname, NULL, NULL);
	if (result == ISC_R_SUCCESS)
		return (true);

	if (result == DNS_R_NXRRSET) {
		result = dns_db_find(db, name, version, dns_rdatatype_aaaa,
				     0, 0, NULL, foundname, NULL, NULL);
		if (result == ISC_R_SUCCESS)
			return (true);
	}

	if (result == DNS_R_NXRRSET || result == DNS_R_NXDOMAIN ||
	    result == DNS_R_EMPTYNAME) {
		if (logit) {
			dns_name_format(name, namebuf, sizeof namebuf);
			dns_zone_log(zone, level, "NS '%s' has no address "
				     "records (A or AAAA)", namebuf);
		}
		return (false);
	}

	if (result == DNS_R_CNAME) {
		if (logit) {
			dns_name_format(name, namebuf, sizeof namebuf);
			dns_zone_log(zone, level, "NS '%s' is a CNAME "
				     "(illegal)", namebuf);
		}
		return (false);
	}

	if (result == DNS_R_DNAME) {
		if (logit) {
			dns_name_format(name, namebuf, sizeof namebuf);
			dns_name_format(foundname, altbuf, sizeof altbuf);
			dns_zone_log(zone, level, "NS '%s' is below a DNAME "
				     "'%s' (illegal)", namebuf, altbuf);
		}
		return (false);
	}

	return (true);
}