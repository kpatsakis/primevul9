notify_isself(dns_zone_t *zone, isc_sockaddr_t *dst) {
	dns_tsigkey_t *key = NULL;
	isc_sockaddr_t src;
	isc_sockaddr_t any;
	bool isself;
	isc_netaddr_t dstaddr;
	isc_result_t result;

	if (zone->view == NULL || zone->isself == NULL)
		return (false);

	switch (isc_sockaddr_pf(dst)) {
	case PF_INET:
		src = zone->notifysrc4;
		isc_sockaddr_any(&any);
		break;
	case PF_INET6:
		src = zone->notifysrc6;
		isc_sockaddr_any6(&any);
		break;
	default:
		return (false);
	}

	/*
	 * When sending from any the kernel will assign a source address
	 * that matches the destination address.
	 */
	if (isc_sockaddr_eqaddr(&any, &src))
		src = *dst;

	isc_netaddr_fromsockaddr(&dstaddr, dst);
	result = dns_view_getpeertsig(zone->view, &dstaddr, &key);
	if (result != ISC_R_SUCCESS && result != ISC_R_NOTFOUND)
		return (false);
	isself = (zone->isself)(zone->view, key, &src, dst, zone->rdclass,
				zone->isselfarg);
	if (key != NULL)
		dns_tsigkey_detach(&key);
	return (isself);
}