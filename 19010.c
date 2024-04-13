respip_enter_rr(struct regional* region, struct resp_addr* raddr,
		const char* rrstr, const char* netblock)
{
	uint8_t* nm;
	uint16_t rrtype = 0, rrclass = 0;
	time_t ttl = 0;
	uint8_t rr[LDNS_RR_BUF_SIZE];
	uint8_t* rdata = NULL;
	size_t rdata_len = 0;
	char buf[65536];
	char bufshort[64];
	struct packed_rrset_data* pd;
	struct sockaddr* sa;
	int ret;
	if(raddr->action != respip_redirect
		&& raddr->action != respip_inform_redirect) {
		log_err("cannot parse response-ip-data %s: response-ip "
			"action for %s is not redirect", rrstr, netblock);
		return 0;
	}
	ret = snprintf(buf, sizeof(buf), ". %s", rrstr);
	if(ret < 0 || ret >= (int)sizeof(buf)) {
		strlcpy(bufshort, rrstr, sizeof(bufshort));
		log_err("bad response-ip-data: %s...", bufshort);
		return 0;
	}
	if(!rrstr_get_rr_content(buf, &nm, &rrtype, &rrclass, &ttl, rr, sizeof(rr),
		&rdata, &rdata_len)) {
		log_err("bad response-ip-data: %s", rrstr);
		return 0;
	}
	free(nm);
	sa = (struct sockaddr*)&raddr->node.addr;
	if (rrtype == LDNS_RR_TYPE_CNAME && raddr->data) {
		log_err("CNAME response-ip data (%s) can not co-exist with other "
			"response-ip data for netblock %s", rrstr, netblock);
		return 0;
	} else if (raddr->data &&
		raddr->data->rk.type == htons(LDNS_RR_TYPE_CNAME)) {
		log_err("response-ip data (%s) can not be added; CNAME response-ip "
			"data already in place for netblock %s", rrstr, netblock);
		return 0;
	} else if((rrtype != LDNS_RR_TYPE_CNAME) &&
		((sa->sa_family == AF_INET && rrtype != LDNS_RR_TYPE_A) ||
		(sa->sa_family == AF_INET6 && rrtype != LDNS_RR_TYPE_AAAA))) {
		log_err("response-ip data %s record type does not correspond "
			"to netblock %s address family", rrstr, netblock);
		return 0;
	}

	if(!raddr->data) {
		raddr->data = new_rrset(region, rrtype, rrclass);
		if(!raddr->data)
			return 0;
	}
	pd = raddr->data->entry.data;
	return rrset_insert_rr(region, pd, rdata, rdata_len, ttl, rrstr);
}