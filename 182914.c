sendtomaster(dns_forward_t *forward) {
	isc_result_t result;
	isc_sockaddr_t src;
	isc_dscp_t dscp = -1;

	LOCK_ZONE(forward->zone);

	if (DNS_ZONE_FLAG(forward->zone, DNS_ZONEFLG_EXITING)) {
		UNLOCK_ZONE(forward->zone);
		return (ISC_R_CANCELED);
	}

	if (forward->which >= forward->zone->masterscnt) {
		UNLOCK_ZONE(forward->zone);
		return (ISC_R_NOMORE);
	}

	forward->addr = forward->zone->masters[forward->which];
	/*
	 * Always use TCP regardless of whether the original update
	 * used TCP.
	 * XXX The timeout may but a bit small if we are far down a
	 * transfer graph and the master has to try several masters.
	 */
	switch (isc_sockaddr_pf(&forward->addr)) {
	case PF_INET:
		src = forward->zone->xfrsource4;
		dscp = forward->zone->xfrsource4dscp;
		break;
	case PF_INET6:
		src = forward->zone->xfrsource6;
		dscp = forward->zone->xfrsource6dscp;
		break;
	default:
		result = ISC_R_NOTIMPLEMENTED;
		goto unlock;
	}
	result = dns_request_createraw(forward->zone->view->requestmgr,
				       forward->msgbuf,
				       &src, &forward->addr, dscp,
				       forward->options, 15 /* XXX */,
				       0, 0, forward->zone->task,
				       forward_callback, forward,
				       &forward->request);
	if (result == ISC_R_SUCCESS) {
		if (!ISC_LINK_LINKED(forward, link))
			ISC_LIST_APPEND(forward->zone->forwards, forward, link);
	}

 unlock:
	UNLOCK_ZONE(forward->zone);
	return (result);
}