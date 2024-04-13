integrity_checks(dns_zone_t *zone, dns_db_t *db) {
	dns_dbiterator_t *dbiterator = NULL;
	dns_dbnode_t *node = NULL;
	dns_rdataset_t rdataset;
	dns_fixedname_t fixed;
	dns_fixedname_t fixedbottom;
	dns_rdata_mx_t mx;
	dns_rdata_ns_t ns;
	dns_rdata_in_srv_t srv;
	dns_rdata_t rdata;
	dns_name_t *name;
	dns_name_t *bottom;
	isc_result_t result;
	bool ok = true, have_spf, have_txt;

	name = dns_fixedname_initname(&fixed);
	bottom = dns_fixedname_initname(&fixedbottom);
	dns_rdataset_init(&rdataset);
	dns_rdata_init(&rdata);

	result = dns_db_createiterator(db, 0, &dbiterator);
	if (result != ISC_R_SUCCESS)
		return (true);

	result = dns_dbiterator_first(dbiterator);
	while (result == ISC_R_SUCCESS) {
		result = dns_dbiterator_current(dbiterator, &node, name);
		if (result != ISC_R_SUCCESS)
			goto cleanup;

		/*
		 * Is this name visible in the zone?
		 */
		if (!dns_name_issubdomain(name, &zone->origin) ||
		    (dns_name_countlabels(bottom) > 0 &&
		     dns_name_issubdomain(name, bottom)))
			goto next;

		/*
		 * Don't check the NS records at the origin.
		 */
		if (dns_name_equal(name, &zone->origin))
			goto checkfordname;

		result = dns_db_findrdataset(db, node, NULL, dns_rdatatype_ns,
					     0, 0, &rdataset, NULL);
		if (result != ISC_R_SUCCESS)
			goto checkfordname;
		/*
		 * Remember bottom of zone due to NS.
		 */
		dns_name_copy(name, bottom, NULL);

		result = dns_rdataset_first(&rdataset);
		while (result == ISC_R_SUCCESS) {
			dns_rdataset_current(&rdataset, &rdata);
			result = dns_rdata_tostruct(&rdata, &ns, NULL);
			RUNTIME_CHECK(result == ISC_R_SUCCESS);
			if (!zone_check_glue(zone, db, &ns.name, name))
				ok = false;
			dns_rdata_reset(&rdata);
			result = dns_rdataset_next(&rdataset);
		}
		dns_rdataset_disassociate(&rdataset);
		goto next;

 checkfordname:
		result = dns_db_findrdataset(db, node, NULL,
					     dns_rdatatype_dname, 0, 0,
					     &rdataset, NULL);
		if (result == ISC_R_SUCCESS) {
			/*
			 * Remember bottom of zone due to DNAME.
			 */
			dns_name_copy(name, bottom, NULL);
			dns_rdataset_disassociate(&rdataset);
		}

		result = dns_db_findrdataset(db, node, NULL, dns_rdatatype_mx,
					     0, 0, &rdataset, NULL);
		if (result != ISC_R_SUCCESS)
			goto checksrv;
		result = dns_rdataset_first(&rdataset);
		while (result == ISC_R_SUCCESS) {
			dns_rdataset_current(&rdataset, &rdata);
			result = dns_rdata_tostruct(&rdata, &mx, NULL);
			RUNTIME_CHECK(result == ISC_R_SUCCESS);
			if (!zone_check_mx(zone, db, &mx.mx, name))
				ok = false;
			dns_rdata_reset(&rdata);
			result = dns_rdataset_next(&rdataset);
		}
		dns_rdataset_disassociate(&rdataset);

 checksrv:
		if (zone->rdclass != dns_rdataclass_in)
			goto next;
		result = dns_db_findrdataset(db, node, NULL, dns_rdatatype_srv,
					     0, 0, &rdataset, NULL);
		if (result != ISC_R_SUCCESS)
			goto checkspf;
		result = dns_rdataset_first(&rdataset);
		while (result == ISC_R_SUCCESS) {
			dns_rdataset_current(&rdataset, &rdata);
			result = dns_rdata_tostruct(&rdata, &srv, NULL);
			RUNTIME_CHECK(result == ISC_R_SUCCESS);
			if (!zone_check_srv(zone, db, &srv.target, name))
				ok = false;
			dns_rdata_reset(&rdata);
			result = dns_rdataset_next(&rdataset);
		}
		dns_rdataset_disassociate(&rdataset);

 checkspf:
		/*
		 * Check if there is a type SPF record without an
		 * SPF-formatted type TXT record also being present.
		 */
		if (!DNS_ZONE_OPTION(zone, DNS_ZONEOPT_CHECKSPF))
			goto next;
		if (zone->rdclass != dns_rdataclass_in)
			goto next;
		have_spf = have_txt = false;
		result = dns_db_findrdataset(db, node, NULL, dns_rdatatype_spf,
					     0, 0, &rdataset, NULL);
		if (result == ISC_R_SUCCESS) {
			dns_rdataset_disassociate(&rdataset);
			have_spf = true;
		}
		result = dns_db_findrdataset(db, node, NULL, dns_rdatatype_txt,
					     0, 0, &rdataset, NULL);
		if (result != ISC_R_SUCCESS)
			goto notxt;
		result = dns_rdataset_first(&rdataset);
		while (result == ISC_R_SUCCESS) {
			dns_rdataset_current(&rdataset, &rdata);
			have_txt = isspf(&rdata);
			dns_rdata_reset(&rdata);
			if (have_txt)
				break;
			result = dns_rdataset_next(&rdataset);
		}
		dns_rdataset_disassociate(&rdataset);

 notxt:
		if (have_spf && !have_txt) {
			char namebuf[DNS_NAME_FORMATSIZE];

			dns_name_format(name, namebuf, sizeof(namebuf));
			dns_zone_log(zone, ISC_LOG_WARNING, "'%s' found type "
				     "SPF record but no SPF TXT record found, "
				     "add matching type TXT record", namebuf);
		}

 next:
		dns_db_detachnode(db, &node);
		result = dns_dbiterator_next(dbiterator);
	}

 cleanup:
	if (node != NULL)
		dns_db_detachnode(db, &node);
	dns_dbiterator_destroy(&dbiterator);

	return (ok);
}