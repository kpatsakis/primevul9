respip_inform_print(struct respip_addr_info* respip_addr, uint8_t* qname,
	uint16_t qtype, uint16_t qclass, struct local_rrset* local_alias,
	struct comm_reply* repinfo)
{
	char srcip[128], respip[128], txt[512];
	unsigned port;

	if(local_alias)
		qname = local_alias->rrset->rk.dname;
	port = (unsigned)((repinfo->addr.ss_family == AF_INET) ?
		ntohs(((struct sockaddr_in*)&repinfo->addr)->sin_port) :
		ntohs(((struct sockaddr_in6*)&repinfo->addr)->sin6_port));
	addr_to_str(&repinfo->addr, repinfo->addrlen, srcip, sizeof(srcip));
	addr_to_str(&respip_addr->addr, respip_addr->addrlen,
		respip, sizeof(respip));
	snprintf(txt, sizeof(txt), "%s/%d inform %s@%u", respip,
		respip_addr->net, srcip, port);
	log_nametypeclass(0, txt, qname, qtype, qclass);
}