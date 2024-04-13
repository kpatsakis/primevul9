dns_zone_setview_helper(dns_zone_t *zone, dns_view_t *view) {
	char namebuf[1024];

	if (zone->prev_view == NULL && zone->view != NULL) {
		dns_view_weakattach(zone->view, &zone->prev_view);
	}

	INSIST(zone != zone->raw);
	if (zone->view != NULL) {
		dns_view_weakdetach(&zone->view);
	}
	dns_view_weakattach(view, &zone->view);

	if (zone->strviewname != NULL) {
		isc_mem_free(zone->mctx, zone->strviewname);
	}
	if (zone->strnamerd != NULL) {
		isc_mem_free(zone->mctx, zone->strnamerd);
	}

	zone_namerd_tostr(zone, namebuf, sizeof namebuf);
	zone->strnamerd = isc_mem_strdup(zone->mctx, namebuf);
	zone_viewname_tostr(zone, namebuf, sizeof namebuf);
	zone->strviewname = isc_mem_strdup(zone->mctx, namebuf);

	if (inline_secure(zone)) {
		dns_zone_setview(zone->raw, view);
	}
}