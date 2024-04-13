ns_zone_reusable(dns_zone_t *zone, const cfg_obj_t *zconfig) {
	const cfg_obj_t *zoptions = NULL;
	const cfg_obj_t *obj = NULL;
	const char *cfilename;
	const char *zfilename;
	dns_zone_t *raw = NULL;
	bool has_raw;
	dns_zonetype_t ztype;

	zoptions = cfg_tuple_get(zconfig, "options");

	/*
	 * We always reconfigure a static-stub zone for simplicity, assuming
	 * the amount of data to be loaded is small.
	 */
	if (zonetype_fromconfig(zoptions) == dns_zone_staticstub) {
		dns_zone_log(zone, ISC_LOG_DEBUG(1),
			     "not reusable: staticstub");
		return (false);
	}

	/* If there's a raw zone, use that for filename and type comparison */
	dns_zone_getraw(zone, &raw);
	if (raw != NULL) {
		zfilename = dns_zone_getfile(raw);
		ztype = dns_zone_gettype(raw);
		dns_zone_detach(&raw);
		has_raw = true;
	} else {
		zfilename = dns_zone_getfile(zone);
		ztype = dns_zone_gettype(zone);
		has_raw = false;
	}

	obj = NULL;
	(void)cfg_map_get(zoptions, "inline-signing", &obj);
	if ((obj == NULL || !cfg_obj_asboolean(obj)) && has_raw) {
		dns_zone_log(zone, ISC_LOG_DEBUG(1),
			     "not reusable: old zone was inline-signing");
		return (false);
	} else if ((obj != NULL && cfg_obj_asboolean(obj)) && !has_raw) {
		dns_zone_log(zone, ISC_LOG_DEBUG(1),
			     "not reusable: old zone was not inline-signing");
		return (false);
	}

	if (zonetype_fromconfig(zoptions) != ztype) {
		dns_zone_log(zone, ISC_LOG_DEBUG(1),
			     "not reusable: type mismatch");
		return (false);
	}

	obj = NULL;
	(void)cfg_map_get(zoptions, "file", &obj);
	if (obj != NULL)
		cfilename = cfg_obj_asstring(obj);
	else
		cfilename = NULL;
	if (!((cfilename == NULL && zfilename == NULL) ||
	      (cfilename != NULL && zfilename != NULL &&
	       strcmp(cfilename, zfilename) == 0)))
	{
		dns_zone_log(zone, ISC_LOG_DEBUG(1),
			     "not reusable: filename mismatch");
		return (false);
	}

	return (true);
}