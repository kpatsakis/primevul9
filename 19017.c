respip_data_cfg(struct respip_set* set, const char* ipstr, const char* rrstr)
{
	struct resp_addr* node;

	node=respip_find_or_create(set, ipstr, 0);
	if(!node || node->action == respip_none) {
		log_err("cannot parse response-ip-data %s: "
			"response-ip node for %s not found", rrstr, ipstr);
		return 0;
	}
	return respip_enter_rr(set->region, node, rrstr, ipstr);
}