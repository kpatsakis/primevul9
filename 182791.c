dns_zone_setdialup(dns_zone_t *zone, dns_dialuptype_t dialup) {
	REQUIRE(DNS_ZONE_VALID(zone));

	LOCK_ZONE(zone);
	DNS_ZONE_CLRFLAG(zone, DNS_ZONEFLG_DIALNOTIFY |
			 DNS_ZONEFLG_DIALREFRESH |
			 DNS_ZONEFLG_NOREFRESH);
	switch (dialup) {
	case dns_dialuptype_no:
		break;
	case dns_dialuptype_yes:
		DNS_ZONE_SETFLAG(zone,	(DNS_ZONEFLG_DIALNOTIFY |
				 DNS_ZONEFLG_DIALREFRESH |
				 DNS_ZONEFLG_NOREFRESH));
		break;
	case dns_dialuptype_notify:
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_DIALNOTIFY);
		break;
	case dns_dialuptype_notifypassive:
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_DIALNOTIFY);
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NOREFRESH);
		break;
	case dns_dialuptype_refresh:
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_DIALREFRESH);
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NOREFRESH);
		break;
	case dns_dialuptype_passive:
		DNS_ZONE_SETFLAG(zone, DNS_ZONEFLG_NOREFRESH);
		break;
	default:
		INSIST(0);
		ISC_UNREACHABLE();
	}
	UNLOCK_ZONE(zone);
}