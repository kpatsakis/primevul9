respip_find_or_create(struct respip_set* set, const char* ipstr, int create)
{
	struct resp_addr* node;
	struct sockaddr_storage addr;
	int net;
	socklen_t addrlen;

	if(!netblockstrtoaddr(ipstr, 0, &addr, &addrlen, &net)) {
		log_err("cannot parse netblock: '%s'", ipstr);
		return NULL;
	}
	node = (struct resp_addr*)addr_tree_find(&set->ip_tree, &addr, addrlen, net);
	if(!node && create) {
		node = regional_alloc_zero(set->region, sizeof(*node));
		if(!node) {
			log_err("out of memory");
			return NULL;
		}
		node->action = respip_none;
		if(!addr_tree_insert(&set->ip_tree, &node->node, &addr,
			addrlen, net)) {
			/* We know we didn't find it, so this should be
			 * impossible. */
			log_warn("unexpected: duplicate address: %s", ipstr);
		}
	}
	return node;
}