configure_zone_acl(const cfg_obj_t *zconfig, const cfg_obj_t *vconfig,
		   const cfg_obj_t *config, acl_type_t acltype,
		   cfg_aclconfctx_t *actx, dns_zone_t *zone,
		   void (*setzacl)(dns_zone_t *, dns_acl_t *),
		   void (*clearzacl)(dns_zone_t *))
{
	isc_result_t result;
	const cfg_obj_t *maps[5] = {NULL, NULL, NULL, NULL, NULL};
	const cfg_obj_t *aclobj = NULL;
	int i = 0;
	dns_acl_t **aclp = NULL, *acl = NULL;
	const char *aclname;
	dns_view_t *view;

	view = dns_zone_getview(zone);

	switch (acltype) {
	    case allow_notify:
		if (view != NULL)
			aclp = &view->notifyacl;
		aclname = "allow-notify";
		break;
	    case allow_query:
		if (view != NULL)
			aclp = &view->queryacl;
		aclname = "allow-query";
		break;
	    case allow_query_on:
		if (view != NULL)
			aclp = &view->queryonacl;
		aclname = "allow-query-on";
		break;
	    case allow_transfer:
		if (view != NULL)
			aclp = &view->transferacl;
		aclname = "allow-transfer";
		break;
	    case allow_update:
		if (view != NULL)
			aclp = &view->updateacl;
		aclname = "allow-update";
		break;
	    case allow_update_forwarding:
		if (view != NULL)
			aclp = &view->upfwdacl;
		aclname = "allow-update-forwarding";
		break;
	    default:
		INSIST(0);
		ISC_UNREACHABLE();
	}

	/* First check to see if ACL is defined within the zone */
	if (zconfig != NULL) {
		maps[0] = cfg_tuple_get(zconfig, "options");
		(void)ns_config_get(maps, aclname, &aclobj);
		if (aclobj != NULL) {
			aclp = NULL;
			goto parse_acl;
		}
	}

	/* Failing that, see if there's a default ACL already in the view */
	if (aclp != NULL && *aclp != NULL) {
		(*setzacl)(zone, *aclp);
		return (ISC_R_SUCCESS);
	}

	/* Check for default ACLs that haven't been parsed yet */
	if (vconfig != NULL) {
		const cfg_obj_t *options = cfg_tuple_get(vconfig, "options");
		if (options != NULL)
			maps[i++] = options;
	}
	if (config != NULL) {
		const cfg_obj_t *options = NULL;
		(void)cfg_map_get(config, "options", &options);
		if (options != NULL)
			maps[i++] = options;
	}
	maps[i++] = ns_g_defaults;
	maps[i] = NULL;

	(void)ns_config_get(maps, aclname, &aclobj);
	if (aclobj == NULL) {
		(*clearzacl)(zone);
		return (ISC_R_SUCCESS);
	}

parse_acl:
	result = cfg_acl_fromconfig(aclobj, config, ns_g_lctx, actx,
				    dns_zone_getmctx(zone), 0, &acl);
	if (result != ISC_R_SUCCESS)
		return (result);
	(*setzacl)(zone, acl);

	/* Set the view default now */
	if (aclp != NULL)
		dns_acl_attach(acl, aclp);

	dns_acl_detach(&acl);
	return (ISC_R_SUCCESS);
}