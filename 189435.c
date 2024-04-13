configure_staticstub_servernames(const cfg_obj_t *zconfig, dns_zone_t *zone,
				 dns_rdatalist_t *rdatalist, const char *zname)
{
	const cfg_listelt_t *element;
	isc_mem_t *mctx = dns_zone_getmctx(zone);
	dns_rdata_t *rdata;
	isc_region_t sregion, region;
	isc_result_t result = ISC_R_SUCCESS;

	for (element = cfg_list_first(zconfig);
	     element != NULL;
	     element = cfg_list_next(element))
	{
		const cfg_obj_t *obj;
		const char *str;
		dns_fixedname_t fixed_name;
		dns_name_t *nsname;
		isc_buffer_t b;

		obj = cfg_listelt_value(element);
		str = cfg_obj_asstring(obj);

		nsname = dns_fixedname_initname(&fixed_name);

		isc_buffer_constinit(&b, str, strlen(str));
		isc_buffer_add(&b, strlen(str));
		result = dns_name_fromtext(nsname, &b, dns_rootname, 0, NULL);
		if (result != ISC_R_SUCCESS) {
			cfg_obj_log(zconfig, ns_g_lctx, ISC_LOG_ERROR,
					    "server-name '%s' is not a valid "
					    "name", str);
			return (result);
		}
		if (dns_name_issubdomain(nsname, dns_zone_getorigin(zone))) {
			cfg_obj_log(zconfig, ns_g_lctx, ISC_LOG_ERROR,
				    "server-name '%s' must not be a "
				    "subdomain of zone name '%s'",
				    str, zname);
			return (ISC_R_FAILURE);
		}

		dns_name_toregion(nsname, &sregion);
		rdata = isc_mem_get(mctx, sizeof(*rdata) + sregion.length);
		if (rdata == NULL)
			return (ISC_R_NOMEMORY);
		region.length = sregion.length;
		region.base = (unsigned char *)(rdata + 1);
		memmove(region.base, sregion.base, region.length);
		dns_rdata_init(rdata);
		dns_rdata_fromregion(rdata, dns_zone_getclass(zone),
				     dns_rdatatype_ns, &region);
		ISC_LIST_APPEND(rdatalist->rdata, rdata, link);
	}

	return (result);
}