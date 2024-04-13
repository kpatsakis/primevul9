ns_zone_configure(const cfg_obj_t *config, const cfg_obj_t *vconfig,
		  const cfg_obj_t *zconfig, cfg_aclconfctx_t *ac,
		  dns_zone_t *zone, dns_zone_t *raw)
{
	isc_result_t result;
	const char *zname;
	dns_rdataclass_t zclass;
	dns_rdataclass_t vclass;
	const cfg_obj_t *maps[5];
	const cfg_obj_t *nodefault[4];
	const cfg_obj_t *zoptions = NULL;
	const cfg_obj_t *options = NULL;
	const cfg_obj_t *obj;
	const char *filename = NULL;
	const char *dupcheck;
	dns_notifytype_t notifytype = dns_notifytype_yes;
	uint32_t count;
	unsigned int dbargc;
	char **dbargv;
	static char default_dbtype[] = "rbt";
	static char dlz_dbtype[] = "dlz";
	char *cpval = default_dbtype;
	isc_mem_t *mctx = dns_zone_getmctx(zone);
	dns_dialuptype_t dialup = dns_dialuptype_no;
	dns_zonetype_t ztype;
	int i;
	int32_t journal_size;
	bool multi;
	bool alt;
	dns_view_t *view;
	bool check = false, fail = false;
	bool warn = false, ignore = false;
	bool ixfrdiff;
	dns_masterformat_t masterformat;
	const dns_master_style_t *masterstyle = &dns_master_style_default;
	isc_stats_t *zoneqrystats;
	dns_stats_t *rcvquerystats;
	dns_zonestat_level_t statlevel = dns_zonestat_none;
	int seconds;
	dns_zone_t *mayberaw = (raw != NULL) ? raw : zone;
	isc_dscp_t dscp;

	i = 0;
	if (zconfig != NULL) {
		zoptions = cfg_tuple_get(zconfig, "options");
		nodefault[i] = maps[i] = zoptions;
		i++;
	}
	if (vconfig != NULL) {
		nodefault[i] = maps[i] = cfg_tuple_get(vconfig, "options");
		i++;
	}
	if (config != NULL) {
		(void)cfg_map_get(config, "options", &options);
		if (options != NULL) {
			nodefault[i] = maps[i] = options;
			i++;
		}
	}
	nodefault[i] = NULL;
	maps[i++] = ns_g_defaults;
	maps[i] = NULL;

	if (vconfig != NULL)
		RETERR(ns_config_getclass(cfg_tuple_get(vconfig, "class"),
					  dns_rdataclass_in, &vclass));
	else
		vclass = dns_rdataclass_in;

	/*
	 * Configure values common to all zone types.
	 */

	zname = cfg_obj_asstring(cfg_tuple_get(zconfig, "name"));

	RETERR(ns_config_getclass(cfg_tuple_get(zconfig, "class"),
				  vclass, &zclass));
	dns_zone_setclass(zone, zclass);
	if (raw != NULL)
		dns_zone_setclass(raw, zclass);

	ztype = zonetype_fromconfig(zoptions);
	if (raw != NULL) {
		dns_zone_settype(raw, ztype);
		dns_zone_settype(zone, dns_zone_master);
	} else
		dns_zone_settype(zone, ztype);

	obj = NULL;
	result = cfg_map_get(zoptions, "database", &obj);
	if (result == ISC_R_SUCCESS)
		cpval = isc_mem_strdup(mctx, cfg_obj_asstring(obj));
	if (cpval == NULL)
		return(ISC_R_NOMEMORY);

	obj = NULL;
	result = cfg_map_get(zoptions, "dlz", &obj);
	if (result == ISC_R_SUCCESS) {
		const char *dlzname = cfg_obj_asstring(obj);
		size_t len;

		if (cpval != default_dbtype) {
		       isc_log_write(ns_g_lctx, NS_LOGCATEGORY_GENERAL,
				     NS_LOGMODULE_SERVER, ISC_LOG_ERROR,
				     "zone '%s': both 'database' and 'dlz' "
				     "specified", zname);
		       return (ISC_R_FAILURE);
		}

		len = strlen(dlzname) + 5;
		cpval = isc_mem_allocate(mctx, len);
		if (cpval == NULL)
			return (ISC_R_NOMEMORY);
		snprintf(cpval, len, "dlz %s", dlzname);
	}

	result = strtoargv(mctx, cpval, &dbargc, &dbargv);
	if (result != ISC_R_SUCCESS && cpval != default_dbtype) {
		isc_mem_free(mctx, cpval);
		return (result);
	}

	/*
	 * ANSI C is strange here.  There is no logical reason why (char **)
	 * cannot be promoted automatically to (const char * const *) by the
	 * compiler w/o generating a warning.
	 */
	result = dns_zone_setdbtype(zone, dbargc, (const char * const *)dbargv);
	isc_mem_put(mctx, dbargv, dbargc * sizeof(*dbargv));
	if (cpval != default_dbtype && cpval != dlz_dbtype)
		isc_mem_free(mctx, cpval);
	if (result != ISC_R_SUCCESS)
		return (result);

	obj = NULL;
	result = cfg_map_get(zoptions, "file", &obj);
	if (result == ISC_R_SUCCESS)
		filename = cfg_obj_asstring(obj);

	/*
	 * Unless we're using some alternative database, a master zone
	 * will be needing a master file.
	 */
	if (ztype == dns_zone_master && cpval == default_dbtype &&
	    filename == NULL) {
		isc_log_write(ns_g_lctx, NS_LOGCATEGORY_GENERAL,
			      NS_LOGMODULE_SERVER, ISC_LOG_ERROR,
			      "zone '%s': 'file' not specified",
			      zname);
		return (ISC_R_FAILURE);
	}

	if (ztype == dns_zone_slave)
		masterformat = dns_masterformat_raw;
	else
		masterformat = dns_masterformat_text;
	obj = NULL;
	result = ns_config_get(maps, "masterfile-format", &obj);
	if (result == ISC_R_SUCCESS) {
		const char *masterformatstr = cfg_obj_asstring(obj);

		if (strcasecmp(masterformatstr, "text") == 0) {
			masterformat = dns_masterformat_text;
		} else if (strcasecmp(masterformatstr, "raw") == 0) {
			masterformat = dns_masterformat_raw;
		} else if (strcasecmp(masterformatstr, "map") == 0) {
			masterformat = dns_masterformat_map;
		} else {
			INSIST(0);
			ISC_UNREACHABLE();
		}
	}

	obj = NULL;
	result = ns_config_get(maps, "masterfile-style", &obj);
	if (result == ISC_R_SUCCESS) {
		const char *masterstylestr = cfg_obj_asstring(obj);

		if (masterformat != dns_masterformat_text) {
			cfg_obj_log(obj, ns_g_lctx, ISC_LOG_ERROR,
				    "zone '%s': 'masterfile-style' "
				    "can only be used with "
				    "'masterfile-format text'", zname);
			return (ISC_R_FAILURE);
		}

		if (strcasecmp(masterstylestr, "full") == 0) {
			masterstyle = &dns_master_style_full;
		} else if (strcasecmp(masterstylestr, "relative") == 0) {
			masterstyle = &dns_master_style_default;
		} else {
			INSIST(0);
			ISC_UNREACHABLE();
		}
	}

	obj = NULL;
	result = ns_config_get(maps, "max-zone-ttl", &obj);
	if (result == ISC_R_SUCCESS && masterformat == dns_masterformat_map) {
		isc_log_write(ns_g_lctx, NS_LOGCATEGORY_GENERAL,
			      NS_LOGMODULE_SERVER, ISC_LOG_ERROR,
			      "zone '%s': 'max-zone-ttl' is not compatible "
			      "with 'masterfile-format map'", zname);
		return (ISC_R_FAILURE);
	} else if (result == ISC_R_SUCCESS) {
		dns_ttl_t maxttl = 0;	/* unlimited */

		if (cfg_obj_isuint32(obj))
			maxttl = cfg_obj_asuint32(obj);
		dns_zone_setmaxttl(zone, maxttl);
		if (raw != NULL)
			dns_zone_setmaxttl(raw, maxttl);
	}

	obj = NULL;
	result = ns_config_get(maps, "max-records", &obj);
	INSIST(result == ISC_R_SUCCESS && obj != NULL);
	dns_zone_setmaxrecords(mayberaw, cfg_obj_asuint32(obj));
	if (zone != mayberaw)
		dns_zone_setmaxrecords(zone, 0);

	if (raw != NULL && filename != NULL) {
#define SIGNED ".signed"
		size_t signedlen = strlen(filename) + sizeof(SIGNED);
		char *signedname;

		RETERR(dns_zone_setfile3(raw, filename,
					 masterformat, masterstyle));
		signedname = isc_mem_get(mctx, signedlen);
		if (signedname == NULL)
			return (ISC_R_NOMEMORY);

		(void)snprintf(signedname, signedlen, "%s" SIGNED, filename);
		result = dns_zone_setfile3(zone, signedname,
					   dns_masterformat_raw, NULL);
		isc_mem_put(mctx, signedname, signedlen);
		if (result != ISC_R_SUCCESS)
			return (result);
	} else
		RETERR(dns_zone_setfile3(zone, filename,
					 masterformat, masterstyle));

	obj = NULL;
	result = cfg_map_get(zoptions, "journal", &obj);
	if (result == ISC_R_SUCCESS)
		RETERR(dns_zone_setjournal(mayberaw, cfg_obj_asstring(obj)));

	/*
	 * Notify messages are processed by the raw zone if it exists.
	 */
	if (ztype == dns_zone_slave)
		RETERR(configure_zone_acl(zconfig, vconfig, config,
					  allow_notify, ac, mayberaw,
					  dns_zone_setnotifyacl,
					  dns_zone_clearnotifyacl));

	/*
	 * XXXAG This probably does not make sense for stubs.
	 */
	RETERR(configure_zone_acl(zconfig, vconfig, config,
				  allow_query, ac, zone,
				  dns_zone_setqueryacl,
				  dns_zone_clearqueryacl));

	RETERR(configure_zone_acl(zconfig, vconfig, config,
				  allow_query_on, ac, zone,
				  dns_zone_setqueryonacl,
				  dns_zone_clearqueryonacl));

	obj = NULL;
	result = ns_config_get(maps, "dialup", &obj);
	INSIST(result == ISC_R_SUCCESS && obj != NULL);
	if (cfg_obj_isboolean(obj)) {
		if (cfg_obj_asboolean(obj))
			dialup = dns_dialuptype_yes;
		else
			dialup = dns_dialuptype_no;
	} else {
		const char *dialupstr = cfg_obj_asstring(obj);
		if (strcasecmp(dialupstr, "notify") == 0) {
			dialup = dns_dialuptype_notify;
		} else if (strcasecmp(dialupstr, "notify-passive") == 0) {
			dialup = dns_dialuptype_notifypassive;
		} else if (strcasecmp(dialupstr, "refresh") == 0) {
			dialup = dns_dialuptype_refresh;
		} else if (strcasecmp(dialupstr, "passive") == 0) {
			dialup = dns_dialuptype_passive;
		} else {
			INSIST(0);
			ISC_UNREACHABLE();
		}
	}
	if (raw != NULL)
		dns_zone_setdialup(raw, dialup);
	dns_zone_setdialup(zone, dialup);

	obj = NULL;
	result = ns_config_get(maps, "zone-statistics", &obj);
	INSIST(result == ISC_R_SUCCESS && obj != NULL);
	if (cfg_obj_isboolean(obj)) {
		if (cfg_obj_asboolean(obj))
			statlevel = dns_zonestat_full;
		else
			statlevel = dns_zonestat_none;
	} else {
		const char *levelstr = cfg_obj_asstring(obj);
		if (strcasecmp(levelstr, "full") == 0) {
			statlevel = dns_zonestat_full;
		} else if (strcasecmp(levelstr, "terse") == 0) {
			statlevel = dns_zonestat_terse;
		} else if (strcasecmp(levelstr, "none") == 0) {
			statlevel = dns_zonestat_none;
		} else {
			INSIST(0);
			ISC_UNREACHABLE();
		}
	}
	dns_zone_setstatlevel(zone, statlevel);

	zoneqrystats  = NULL;
	rcvquerystats = NULL;
	if (statlevel == dns_zonestat_full) {
		RETERR(isc_stats_create(mctx, &zoneqrystats,
					dns_nsstatscounter_max));
		RETERR(dns_rdatatypestats_create(mctx,
					&rcvquerystats));
	}
	dns_zone_setrequeststats(zone,  zoneqrystats);
	dns_zone_setrcvquerystats(zone, rcvquerystats);

	if (zoneqrystats != NULL)
		isc_stats_detach(&zoneqrystats);

	if(rcvquerystats != NULL)
		dns_stats_detach(&rcvquerystats);

	/*
	 * Configure master functionality.  This applies
	 * to primary masters (type "master") and slaves
	 * acting as masters (type "slave"), but not to stubs.
	 */
	if (ztype != dns_zone_stub && ztype != dns_zone_staticstub &&
	    ztype != dns_zone_redirect) {
		obj = NULL;
		result = ns_config_get(maps, "notify", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		if (cfg_obj_isboolean(obj)) {
			if (cfg_obj_asboolean(obj))
				notifytype = dns_notifytype_yes;
			else
				notifytype = dns_notifytype_no;
		} else {
			const char *notifystr = cfg_obj_asstring(obj);
			if (strcasecmp(notifystr, "explicit") == 0) {
				notifytype = dns_notifytype_explicit;
			} else if (strcasecmp(notifystr, "master-only") == 0) {
				notifytype = dns_notifytype_masteronly;
			} else {
				INSIST(0);
				ISC_UNREACHABLE();
			}
		}
		if (raw != NULL)
			dns_zone_setnotifytype(raw, dns_notifytype_no);
		dns_zone_setnotifytype(zone, notifytype);

		obj = NULL;
		result = ns_config_get(maps, "also-notify", &obj);
		if (result == ISC_R_SUCCESS &&
		    (notifytype == dns_notifytype_yes ||
		     notifytype == dns_notifytype_explicit ||
		     (notifytype == dns_notifytype_masteronly &&
		      ztype == dns_zone_master)))
		{
			dns_ipkeylist_t ipkl;
			dns_ipkeylist_init(&ipkl);

			RETERR(ns_config_getipandkeylist(config, obj, mctx,
							 &ipkl));
			result = dns_zone_setalsonotifydscpkeys(zone,
								ipkl.addrs,
								ipkl.dscps,
								ipkl.keys,
								ipkl.count);
			dns_ipkeylist_clear(mctx, &ipkl);
			RETERR(result);
		} else
			RETERR(dns_zone_setalsonotify(zone, NULL, 0));

		obj = NULL;
		result = ns_config_get(maps, "notify-source", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		RETERR(dns_zone_setnotifysrc4(zone, cfg_obj_assockaddr(obj)));
		dscp = cfg_obj_getdscp(obj);
		if (dscp == -1)
			dscp = ns_g_dscp;
		RETERR(dns_zone_setnotifysrc4dscp(zone, dscp));
		ns_add_reserved_dispatch(ns_g_server, cfg_obj_assockaddr(obj));

		obj = NULL;
		result = ns_config_get(maps, "notify-source-v6", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		RETERR(dns_zone_setnotifysrc6(zone, cfg_obj_assockaddr(obj)));
		dscp = cfg_obj_getdscp(obj);
		if (dscp == -1)
			dscp = ns_g_dscp;
		RETERR(dns_zone_setnotifysrc6dscp(zone, dscp));
		ns_add_reserved_dispatch(ns_g_server, cfg_obj_assockaddr(obj));

		obj = NULL;
		result = ns_config_get(maps, "notify-to-soa", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setoption(zone, DNS_ZONEOPT_NOTIFYTOSOA,
				   cfg_obj_asboolean(obj));

		dns_zone_setisself(zone, ns_client_isself, NULL);

		RETERR(configure_zone_acl(zconfig, vconfig, config,
					  allow_transfer, ac, zone,
					  dns_zone_setxfracl,
					  dns_zone_clearxfracl));

		obj = NULL;
		result = ns_config_get(maps, "max-transfer-time-out", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setmaxxfrout(zone, cfg_obj_asuint32(obj) * 60);

		obj = NULL;
		result = ns_config_get(maps, "max-transfer-idle-out", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setidleout(zone, cfg_obj_asuint32(obj) * 60);

		obj = NULL;
		result = ns_config_get(maps, "max-journal-size", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		if (raw != NULL)
			dns_zone_setjournalsize(raw, -1);
		dns_zone_setjournalsize(zone, -1);
		if (cfg_obj_isstring(obj)) {
			const char *str = cfg_obj_asstring(obj);
			INSIST(strcasecmp(str, "unlimited") == 0);
			journal_size = UINT32_MAX / 2;
		} else {
			isc_resourcevalue_t value;
			value = cfg_obj_asuint64(obj);
			if (value > UINT32_MAX / 2) {
				cfg_obj_log(obj, ns_g_lctx,
					    ISC_LOG_ERROR,
					    "'max-journal-size "
					    "%" PRId64 "' "
					    "is too large",
					    value);
				RETERR(ISC_R_RANGE);
			}
			journal_size = (uint32_t)value;
		}
		if (raw != NULL)
			dns_zone_setjournalsize(raw, journal_size);
		dns_zone_setjournalsize(zone, journal_size);

		obj = NULL;
		result = ns_config_get(maps, "ixfr-from-differences", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		if (cfg_obj_isboolean(obj))
			ixfrdiff = cfg_obj_asboolean(obj);
		else if (!strcasecmp(cfg_obj_asstring(obj), "master") &&
			 ztype == dns_zone_master)
			ixfrdiff = true;
		else if (!strcasecmp(cfg_obj_asstring(obj), "slave") &&
			ztype == dns_zone_slave)
			ixfrdiff = true;
		else
			ixfrdiff = false;
		if (raw != NULL) {
			dns_zone_setoption(raw, DNS_ZONEOPT_IXFRFROMDIFFS,
					   true);
			dns_zone_setoption(zone, DNS_ZONEOPT_IXFRFROMDIFFS,
					   false);
		} else
			dns_zone_setoption(zone, DNS_ZONEOPT_IXFRFROMDIFFS,
					   ixfrdiff);

		obj = NULL;
		result = ns_config_get(maps, "request-expire", &obj);
		INSIST(result == ISC_R_SUCCESS);
		dns_zone_setrequestexpire(zone, cfg_obj_asboolean(obj));

		obj = NULL;
		result = ns_config_get(maps, "request-ixfr", &obj);
		INSIST(result == ISC_R_SUCCESS);
		dns_zone_setrequestixfr(zone, cfg_obj_asboolean(obj));

		obj = NULL;
		checknames(ztype, maps, &obj);
		INSIST(obj != NULL);
		if (strcasecmp(cfg_obj_asstring(obj), "warn") == 0) {
			fail = false;
			check = true;
		} else if (strcasecmp(cfg_obj_asstring(obj), "fail") == 0) {
			fail = check = true;
		} else if (strcasecmp(cfg_obj_asstring(obj), "ignore") == 0) {
			fail = check = false;
		} else {
			INSIST(0);
			ISC_UNREACHABLE();
		}
		if (raw != NULL) {
			dns_zone_setoption(raw, DNS_ZONEOPT_CHECKNAMES,
					   check);
			dns_zone_setoption(raw, DNS_ZONEOPT_CHECKNAMESFAIL,
					   fail);
			dns_zone_setoption(zone, DNS_ZONEOPT_CHECKNAMES,
					   false);
			dns_zone_setoption(zone, DNS_ZONEOPT_CHECKNAMESFAIL,
					   false);
		} else {
			dns_zone_setoption(zone, DNS_ZONEOPT_CHECKNAMES,
					   check);
			dns_zone_setoption(zone, DNS_ZONEOPT_CHECKNAMESFAIL,
					   fail);
		}

		obj = NULL;
		result = ns_config_get(maps, "notify-delay", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setnotifydelay(zone, cfg_obj_asuint32(obj));

		obj = NULL;
		result = ns_config_get(maps, "check-sibling", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setoption(zone, DNS_ZONEOPT_CHECKSIBLING,
				   cfg_obj_asboolean(obj));

		obj = NULL;
		result = ns_config_get(maps, "check-spf", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		if (strcasecmp(cfg_obj_asstring(obj), "warn") == 0) {
			check = true;
		} else if (strcasecmp(cfg_obj_asstring(obj), "ignore") == 0) {
			check = false;
		} else {
			INSIST(0);
			ISC_UNREACHABLE();
		}
		dns_zone_setoption(zone, DNS_ZONEOPT_CHECKSPF, check);

		obj = NULL;
		result = ns_config_get(maps, "zero-no-soa-ttl", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setzeronosoattl(zone, cfg_obj_asboolean(obj));

		obj = NULL;
		result = ns_config_get(maps, "nsec3-test-zone", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setoption(zone, DNS_ZONEOPT_NSEC3TESTZONE,
				   cfg_obj_asboolean(obj));
	} else if (ztype == dns_zone_redirect) {
		dns_zone_setnotifytype(zone, dns_notifytype_no);

		obj = NULL;
		result = ns_config_get(maps, "max-journal-size", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setjournalsize(zone, -1);
		if (cfg_obj_isstring(obj)) {
			const char *str = cfg_obj_asstring(obj);
			INSIST(strcasecmp(str, "unlimited") == 0);
			journal_size = UINT32_MAX / 2;
		} else {
			isc_resourcevalue_t value;
			value = cfg_obj_asuint64(obj);
			if (value > UINT32_MAX / 2) {
				cfg_obj_log(obj, ns_g_lctx,
					    ISC_LOG_ERROR,
					    "'max-journal-size "
					    "%" PRId64 "' "
					    "is too large",
					    value);
				RETERR(ISC_R_RANGE);
			}
			journal_size = (uint32_t)value;
		}
		dns_zone_setjournalsize(zone, journal_size);
	}

	/*
	 * Configure update-related options.  These apply to
	 * primary masters only.
	 */
	if (ztype == dns_zone_master) {
		dns_acl_t *updateacl;

		RETERR(configure_zone_acl(zconfig, vconfig, config,
					  allow_update, ac, mayberaw,
					  dns_zone_setupdateacl,
					  dns_zone_clearupdateacl));

		updateacl = dns_zone_getupdateacl(mayberaw);
		if (updateacl != NULL  && dns_acl_isinsecure(updateacl))
			isc_log_write(ns_g_lctx, DNS_LOGCATEGORY_SECURITY,
				      NS_LOGMODULE_SERVER, ISC_LOG_WARNING,
				      "zone '%s' allows unsigned updates "
				      "from remote hosts, which is insecure",
				      zname);

		RETERR(configure_zone_ssutable(zoptions, mayberaw, zname));
	}

	if (ztype == dns_zone_master || raw != NULL) {
		const cfg_obj_t *validity, *resign;
		bool allow = false, maint = false;

		obj = NULL;
		result = ns_config_get(maps, "sig-validity-interval", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);

		validity = cfg_tuple_get(obj, "validity");
		seconds = cfg_obj_asuint32(validity);
		if (!ns_g_sigvalinsecs) {
			seconds *= 86400;
		}
		dns_zone_setsigvalidityinterval(zone, seconds);

		resign = cfg_tuple_get(obj, "re-sign");
		if (cfg_obj_isvoid(resign)) {
			seconds /= 4;
		} else if (!ns_g_sigvalinsecs) {
			if (seconds > 7 * 86400) {
				seconds = cfg_obj_asuint32(resign) * 86400;
			} else {
				seconds = cfg_obj_asuint32(resign) * 3600;
			}
		} else {
			seconds = cfg_obj_asuint32(resign);
		}
		dns_zone_setsigresigninginterval(zone, seconds);

		obj = NULL;
		result = ns_config_get(maps, "key-directory", &obj);
		if (result == ISC_R_SUCCESS) {
			filename = cfg_obj_asstring(obj);
			RETERR(dns_zone_setkeydirectory(zone, filename));
		}

		obj = NULL;
		result = ns_config_get(maps, "sig-signing-signatures", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setsignatures(zone, cfg_obj_asuint32(obj));

		obj = NULL;
		result = ns_config_get(maps, "sig-signing-nodes", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setnodes(zone, cfg_obj_asuint32(obj));

		obj = NULL;
		result = ns_config_get(maps, "sig-signing-type", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setprivatetype(zone, cfg_obj_asuint32(obj));

		obj = NULL;
		result = ns_config_get(maps, "update-check-ksk", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setoption(zone, DNS_ZONEOPT_UPDATECHECKKSK,
				   cfg_obj_asboolean(obj));

		obj = NULL;
		result = ns_config_get(maps, "dnssec-dnskey-kskonly", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setoption(zone, DNS_ZONEOPT_DNSKEYKSKONLY,
				   cfg_obj_asboolean(obj));

		obj = NULL;
		result = ns_config_get(maps, "dnssec-loadkeys-interval", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		RETERR(dns_zone_setrefreshkeyinterval(zone,
						      cfg_obj_asuint32(obj)));

		obj = NULL;
		result = cfg_map_get(zoptions, "auto-dnssec", &obj);
		if (result == ISC_R_SUCCESS) {
			const char *arg = cfg_obj_asstring(obj);
			if (strcasecmp(arg, "allow") == 0) {
				allow = true;
			} else if (strcasecmp(arg, "maintain") == 0) {
				allow = maint = true;
			} else if (strcasecmp(arg, "off") == 0) {
				/* Default */
			} else {
				INSIST(0);
				ISC_UNREACHABLE();
			}
			dns_zone_setkeyopt(zone, DNS_ZONEKEY_ALLOW, allow);
			dns_zone_setkeyopt(zone, DNS_ZONEKEY_MAINTAIN, maint);
		}
	}

	if (ztype == dns_zone_slave) {
		RETERR(configure_zone_acl(zconfig, vconfig, config,
					  allow_update_forwarding, ac,
					  mayberaw, dns_zone_setforwardacl,
					  dns_zone_clearforwardacl));
	}

	/*%
	 * Primary master functionality.
	 */
	if (ztype == dns_zone_master) {
		obj = NULL;
		result = ns_config_get(maps, "check-wildcard", &obj);
		if (result == ISC_R_SUCCESS)
			check = cfg_obj_asboolean(obj);
		else
			check = false;
		dns_zone_setoption(mayberaw, DNS_ZONEOPT_CHECKWILDCARD, check);

		/*
		 * With map files, the default is ignore duplicate
		 * records.  With other master formats, the default is
		 * taken from the global configuration.
		 */
		obj = NULL;
		if (masterformat != dns_masterformat_map) {
			result = ns_config_get(maps, "check-dup-records", &obj);
			INSIST(result == ISC_R_SUCCESS && obj != NULL);
			dupcheck = cfg_obj_asstring(obj);
		} else {
			result = ns_config_get(nodefault, "check-dup-records",
					       &obj);
			if (result == ISC_R_SUCCESS)
				dupcheck = cfg_obj_asstring(obj);
			else
				dupcheck = "ignore";

		}
		if (strcasecmp(dupcheck, "warn") == 0) {
			fail = false;
			check = true;
		} else if (strcasecmp(dupcheck, "fail") == 0) {
			fail = check = true;
		} else if (strcasecmp(dupcheck, "ignore") == 0) {
			fail = check = false;
		} else {
			INSIST(0);
			ISC_UNREACHABLE();
		}
		dns_zone_setoption(mayberaw, DNS_ZONEOPT_CHECKDUPRR, check);
		dns_zone_setoption(mayberaw, DNS_ZONEOPT_CHECKDUPRRFAIL, fail);

		obj = NULL;
		result = ns_config_get(maps, "check-mx", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		if (strcasecmp(cfg_obj_asstring(obj), "warn") == 0) {
			fail = false;
			check = true;
		} else if (strcasecmp(cfg_obj_asstring(obj), "fail") == 0) {
			fail = check = true;
		} else if (strcasecmp(cfg_obj_asstring(obj), "ignore") == 0) {
			fail = check = false;
		} else {
			INSIST(0);
			ISC_UNREACHABLE();
		}
		dns_zone_setoption(mayberaw, DNS_ZONEOPT_CHECKMX, check);
		dns_zone_setoption(mayberaw, DNS_ZONEOPT_CHECKMXFAIL, fail);

		/*
		 * With map files, the default is *not* to check
		 * integrity.  With other master formats, the default is
		 * taken from the global configuration.
		 */
		obj = NULL;
		if (masterformat != dns_masterformat_map) {
			result = ns_config_get(maps, "check-integrity", &obj);
			INSIST(result == ISC_R_SUCCESS && obj != NULL);
			dns_zone_setoption(mayberaw, DNS_ZONEOPT_CHECKINTEGRITY,
					   cfg_obj_asboolean(obj));
		} else {
			check = false;
			result = ns_config_get(nodefault, "check-integrity",
					       &obj);
			if (result == ISC_R_SUCCESS)
				check = cfg_obj_asboolean(obj);
			dns_zone_setoption(mayberaw, DNS_ZONEOPT_CHECKINTEGRITY,
					   check);
		}

		obj = NULL;
		result = ns_config_get(maps, "check-mx-cname", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		if (strcasecmp(cfg_obj_asstring(obj), "warn") == 0) {
			warn = true;
			ignore = false;
		} else if (strcasecmp(cfg_obj_asstring(obj), "fail") == 0) {
			warn = ignore = false;
		} else if (strcasecmp(cfg_obj_asstring(obj), "ignore") == 0) {
			warn = ignore = true;
		} else {
			INSIST(0);
			ISC_UNREACHABLE();
		}
		dns_zone_setoption(mayberaw, DNS_ZONEOPT_WARNMXCNAME, warn);
		dns_zone_setoption(mayberaw, DNS_ZONEOPT_IGNOREMXCNAME, ignore);

		obj = NULL;
		result = ns_config_get(maps, "check-srv-cname", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		if (strcasecmp(cfg_obj_asstring(obj), "warn") == 0) {
			warn = true;
			ignore = false;
		} else if (strcasecmp(cfg_obj_asstring(obj), "fail") == 0) {
			warn = ignore = false;
		} else if (strcasecmp(cfg_obj_asstring(obj), "ignore") == 0) {
			warn = ignore = true;
		} else {
			INSIST(0);
			ISC_UNREACHABLE();
		}
		dns_zone_setoption(mayberaw, DNS_ZONEOPT_WARNSRVCNAME, warn);
		dns_zone_setoption(mayberaw, DNS_ZONEOPT_IGNORESRVCNAME,
				   ignore);

		obj = NULL;
		result = ns_config_get(maps, "dnssec-secure-to-insecure", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setoption(mayberaw, DNS_ZONEOPT_SECURETOINSECURE,
				   cfg_obj_asboolean(obj));

		obj = NULL;
		result = cfg_map_get(zoptions, "dnssec-update-mode", &obj);
		if (result == ISC_R_SUCCESS) {
			const char *arg = cfg_obj_asstring(obj);
			if (strcasecmp(arg, "no-resign") == 0) {
				dns_zone_setkeyopt(zone, DNS_ZONEKEY_NORESIGN,
						   true);
			} else if (strcasecmp(arg, "maintain") == 0) {
				/* Default */
			} else {
				INSIST(0);
				ISC_UNREACHABLE();
			}
		}

		obj = NULL;
		result = ns_config_get(maps, "serial-update-method", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		if (strcasecmp(cfg_obj_asstring(obj), "unixtime") == 0)
			dns_zone_setserialupdatemethod(zone,
						    dns_updatemethod_unixtime);
		else if (strcasecmp(cfg_obj_asstring(obj), "date") == 0)
			dns_zone_setserialupdatemethod(zone,
						       dns_updatemethod_date);
		else
			dns_zone_setserialupdatemethod(zone,
						  dns_updatemethod_increment);
	}

	/*
	 * Configure slave functionality.
	 */
	switch (ztype) {
	case dns_zone_slave:
	case dns_zone_stub:
	case dns_zone_redirect:
		count = 0;
		obj = NULL;
		(void)cfg_map_get(zoptions, "masters", &obj);
		if (obj != NULL) {
			dns_ipkeylist_t ipkl;
			dns_ipkeylist_init(&ipkl);

			RETERR(ns_config_getipandkeylist(config, obj, mctx,
							 &ipkl));
			result = dns_zone_setmasterswithkeys(mayberaw,
							     ipkl.addrs,
							     ipkl.keys,
							     ipkl.count);
			count = ipkl.count;
			dns_ipkeylist_clear(mctx, &ipkl);
			RETERR(result);
		} else
			result = dns_zone_setmasters(mayberaw, NULL, 0);
		RETERR(result);

		multi = false;
		if (count > 1) {
			obj = NULL;
			result = ns_config_get(maps, "multi-master", &obj);
			INSIST(result == ISC_R_SUCCESS && obj != NULL);
			multi = cfg_obj_asboolean(obj);
		}
		dns_zone_setoption(mayberaw, DNS_ZONEOPT_MULTIMASTER, multi);

		obj = NULL;
		result = ns_config_get(maps, "max-transfer-time-in", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setmaxxfrin(mayberaw, cfg_obj_asuint32(obj) * 60);

		obj = NULL;
		result = ns_config_get(maps, "max-transfer-idle-in", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setidlein(mayberaw, cfg_obj_asuint32(obj) * 60);

		obj = NULL;
		result = ns_config_get(maps, "max-refresh-time", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setmaxrefreshtime(mayberaw, cfg_obj_asuint32(obj));

		obj = NULL;
		result = ns_config_get(maps, "min-refresh-time", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setminrefreshtime(mayberaw, cfg_obj_asuint32(obj));

		obj = NULL;
		result = ns_config_get(maps, "max-retry-time", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setmaxretrytime(mayberaw, cfg_obj_asuint32(obj));

		obj = NULL;
		result = ns_config_get(maps, "min-retry-time", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		dns_zone_setminretrytime(mayberaw, cfg_obj_asuint32(obj));

		obj = NULL;
		result = ns_config_get(maps, "transfer-source", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		RETERR(dns_zone_setxfrsource4(mayberaw,
					      cfg_obj_assockaddr(obj)));
		dscp = cfg_obj_getdscp(obj);
		if (dscp == -1)
			dscp = ns_g_dscp;
		RETERR(dns_zone_setxfrsource4dscp(mayberaw, dscp));
		ns_add_reserved_dispatch(ns_g_server, cfg_obj_assockaddr(obj));

		obj = NULL;
		result = ns_config_get(maps, "transfer-source-v6", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		RETERR(dns_zone_setxfrsource6(mayberaw,
					      cfg_obj_assockaddr(obj)));
		dscp = cfg_obj_getdscp(obj);
		if (dscp == -1)
			dscp = ns_g_dscp;
		RETERR(dns_zone_setxfrsource6dscp(mayberaw, dscp));
		ns_add_reserved_dispatch(ns_g_server, cfg_obj_assockaddr(obj));

		obj = NULL;
		result = ns_config_get(maps, "alt-transfer-source", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		RETERR(dns_zone_setaltxfrsource4(mayberaw,
						 cfg_obj_assockaddr(obj)));
		dscp = cfg_obj_getdscp(obj);
		if (dscp == -1)
			dscp = ns_g_dscp;
		RETERR(dns_zone_setaltxfrsource4dscp(mayberaw, dscp));

		obj = NULL;
		result = ns_config_get(maps, "alt-transfer-source-v6", &obj);
		INSIST(result == ISC_R_SUCCESS && obj != NULL);
		RETERR(dns_zone_setaltxfrsource6(mayberaw,
						 cfg_obj_assockaddr(obj)));
		dscp = cfg_obj_getdscp(obj);
		if (dscp == -1)
			dscp = ns_g_dscp;
		RETERR(dns_zone_setaltxfrsource6dscp(mayberaw, dscp));

		obj = NULL;
		(void)ns_config_get(maps, "use-alt-transfer-source", &obj);
		if (obj == NULL) {
			/*
			 * Default off when views are in use otherwise
			 * on for BIND 8 compatibility.
			 */
			view = dns_zone_getview(zone);
			if (view != NULL && strcmp(view->name, "_default") == 0)
				alt = true;
			else
				alt = false;
		} else
			alt = cfg_obj_asboolean(obj);
		dns_zone_setoption(mayberaw, DNS_ZONEOPT_USEALTXFRSRC, alt);

		obj = NULL;
		(void)ns_config_get(maps, "try-tcp-refresh", &obj);
		dns_zone_setoption(mayberaw, DNS_ZONEOPT_TRYTCPREFRESH,
				   cfg_obj_asboolean(obj));
		break;

	case dns_zone_staticstub:
		RETERR(configure_staticstub(zoptions, zone, zname,
					    default_dbtype));
		break;

	default:
		break;
	}

	return (ISC_R_SUCCESS);
}