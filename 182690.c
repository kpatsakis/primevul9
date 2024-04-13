zone_check_glue(dns_zone_t *zone, dns_db_t *db, dns_name_t *name,
		dns_name_t *owner)
{
	bool answer = true;
	isc_result_t result, tresult;
	char ownerbuf[DNS_NAME_FORMATSIZE];
	char namebuf[DNS_NAME_FORMATSIZE];
	char altbuf[DNS_NAME_FORMATSIZE];
	dns_fixedname_t fixed;
	dns_name_t *foundname;
	dns_rdataset_t a;
	dns_rdataset_t aaaa;
	int level;

	/*
	 * Outside of zone.
	 */
	if (!dns_name_issubdomain(name, &zone->origin)) {
		if (zone->checkns != NULL)
			return ((zone->checkns)(zone, name, owner, NULL, NULL));
		return (true);
	}

	if (zone->type == dns_zone_master)
		level = ISC_LOG_ERROR;
	else
		level = ISC_LOG_WARNING;

	foundname = dns_fixedname_initname(&fixed);
	dns_rdataset_init(&a);
	dns_rdataset_init(&aaaa);

	/*
	 * Perform a regular lookup to catch DNAME records then look
	 * for glue.
	 */
	result = dns_db_find(db, name, NULL, dns_rdatatype_a,
			     0, 0, NULL, foundname, &a, NULL);
	switch (result) {
	case ISC_R_SUCCESS:
	case DNS_R_DNAME:
	case DNS_R_CNAME:
		break;
	default:
		if (dns_rdataset_isassociated(&a))
			dns_rdataset_disassociate(&a);
		result = dns_db_find(db, name, NULL, dns_rdatatype_a,
				     DNS_DBFIND_GLUEOK, 0, NULL,
				     foundname, &a, NULL);
	}
	if (result == ISC_R_SUCCESS) {
		dns_rdataset_disassociate(&a);
		return (true);
	} else if (result == DNS_R_DELEGATION)
		dns_rdataset_disassociate(&a);

	if (result == DNS_R_NXRRSET || result == DNS_R_DELEGATION ||
	    result == DNS_R_GLUE) {
		tresult = dns_db_find(db, name, NULL, dns_rdatatype_aaaa,
				     DNS_DBFIND_GLUEOK, 0, NULL,
				     foundname, &aaaa, NULL);
		if (tresult == ISC_R_SUCCESS) {
			if (dns_rdataset_isassociated(&a))
				dns_rdataset_disassociate(&a);
			dns_rdataset_disassociate(&aaaa);
			return (true);
		}
		if (tresult == DNS_R_DELEGATION || tresult == DNS_R_DNAME)
			dns_rdataset_disassociate(&aaaa);
		if (result == DNS_R_GLUE || tresult == DNS_R_GLUE) {
			/*
			 * Check glue against child zone.
			 */
			if (zone->checkns != NULL)
				answer = (zone->checkns)(zone, name, owner,
							 &a, &aaaa);
			if (dns_rdataset_isassociated(&a))
				dns_rdataset_disassociate(&a);
			if (dns_rdataset_isassociated(&aaaa))
				dns_rdataset_disassociate(&aaaa);
			return (answer);
		}
	}

	dns_name_format(owner, ownerbuf, sizeof ownerbuf);
	dns_name_format(name, namebuf, sizeof namebuf);
	if (result == DNS_R_NXRRSET || result == DNS_R_NXDOMAIN ||
	    result == DNS_R_EMPTYNAME || result == DNS_R_DELEGATION) {
		const char *what;
		bool required = false;
		if (dns_name_issubdomain(name, owner)) {
			what = "REQUIRED GLUE ";
			required = true;
		 } else if (result == DNS_R_DELEGATION)
			what = "SIBLING GLUE ";
		else
			what = "";

		if (result != DNS_R_DELEGATION || required ||
		    DNS_ZONE_OPTION(zone, DNS_ZONEOPT_CHECKSIBLING)) {
			dns_zone_log(zone, level, "%s/NS '%s' has no %s"
				     "address records (A or AAAA)",
				     ownerbuf, namebuf, what);
			/*
			 * Log missing address record.
			 */
			if (result == DNS_R_DELEGATION && zone->checkns != NULL)
				(void)(zone->checkns)(zone, name, owner,
						      &a, &aaaa);
			/* XXX950 make fatal for 9.5.0. */
			/* answer = false; */
		}
	} else if (result == DNS_R_CNAME) {
		dns_zone_log(zone, level, "%s/NS '%s' is a CNAME (illegal)",
			     ownerbuf, namebuf);
		/* XXX950 make fatal for 9.5.0. */
		/* answer = false; */
	} else if (result == DNS_R_DNAME) {
		dns_name_format(foundname, altbuf, sizeof altbuf);
		dns_zone_log(zone, level,
			     "%s/NS '%s' is below a DNAME '%s' (illegal)",
			     ownerbuf, namebuf, altbuf);
		/* XXX950 make fatal for 9.5.0. */
		/* answer = false; */
	}

	if (dns_rdataset_isassociated(&a))
		dns_rdataset_disassociate(&a);
	if (dns_rdataset_isassociated(&aaaa))
		dns_rdataset_disassociate(&aaaa);
	return (answer);
}