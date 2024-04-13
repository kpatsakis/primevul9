zone_check_mx(dns_zone_t *zone, dns_db_t *db, dns_name_t *name,
	      dns_name_t *owner)
{
	isc_result_t result;
	char ownerbuf[DNS_NAME_FORMATSIZE];
	char namebuf[DNS_NAME_FORMATSIZE];
	char altbuf[DNS_NAME_FORMATSIZE];
	dns_fixedname_t fixed;
	dns_name_t *foundname;
	int level;

	/*
	 * "." means the services does not exist.
	 */
	if (dns_name_equal(name, dns_rootname))
		return (true);

	/*
	 * Outside of zone.
	 */
	if (!dns_name_issubdomain(name, &zone->origin)) {
		if (zone->checkmx != NULL)
			return ((zone->checkmx)(zone, name, owner));
		return (true);
	}

	if (zone->type == dns_zone_master)
		level = ISC_LOG_ERROR;
	else
		level = ISC_LOG_WARNING;

	foundname = dns_fixedname_initname(&fixed);

	result = dns_db_find(db, name, NULL, dns_rdatatype_a,
			     0, 0, NULL, foundname, NULL, NULL);
	if (result == ISC_R_SUCCESS)
		return (true);

	if (result == DNS_R_NXRRSET) {
		result = dns_db_find(db, name, NULL, dns_rdatatype_aaaa,
				     0, 0, NULL, foundname, NULL, NULL);
		if (result == ISC_R_SUCCESS)
			return (true);
	}

	dns_name_format(owner, ownerbuf, sizeof ownerbuf);
	dns_name_format(name, namebuf, sizeof namebuf);
	if (result == DNS_R_NXRRSET || result == DNS_R_NXDOMAIN ||
	    result == DNS_R_EMPTYNAME) {
		if (!DNS_ZONE_OPTION(zone, DNS_ZONEOPT_CHECKMXFAIL))
			level = ISC_LOG_WARNING;
		dns_zone_log(zone, level,
			     "%s/MX '%s' has no address records (A or AAAA)",
			     ownerbuf, namebuf);
		return ((level == ISC_LOG_WARNING) ? true : false);
	}

	if (result == DNS_R_CNAME) {
		if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_WARNMXCNAME) ||
		    DNS_ZONE_OPTION(zone, DNS_ZONEOPT_IGNOREMXCNAME))
			level = ISC_LOG_WARNING;
		if (!DNS_ZONE_OPTION(zone, DNS_ZONEOPT_IGNOREMXCNAME))
			dns_zone_log(zone, level,
				     "%s/MX '%s' is a CNAME (illegal)",
				     ownerbuf, namebuf);
		return ((level == ISC_LOG_WARNING) ? true : false);
	}

	if (result == DNS_R_DNAME) {
		if (DNS_ZONE_OPTION(zone, DNS_ZONEOPT_WARNMXCNAME) ||
		    DNS_ZONE_OPTION(zone, DNS_ZONEOPT_IGNOREMXCNAME))
			level = ISC_LOG_WARNING;
		if (!DNS_ZONE_OPTION(zone, DNS_ZONEOPT_IGNOREMXCNAME)) {
			dns_name_format(foundname, altbuf, sizeof altbuf);
			dns_zone_log(zone, level, "%s/MX '%s' is below a DNAME"
				     " '%s' (illegal)", ownerbuf, namebuf,
				     altbuf);
		}
		return ((level == ISC_LOG_WARNING) ? true : false);
	}

	if (zone->checkmx != NULL && result == DNS_R_DELEGATION)
		return ((zone->checkmx)(zone, name, owner));

	return (true);
}