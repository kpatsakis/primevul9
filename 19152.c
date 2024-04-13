bool print_addr_hint_cb(ut64 addr, const RVector/*<const RAnalAddrHintRecord>*/ *records, void *user) {
	HintNode *node = R_NEW0 (HintNode);
	if (!node) {
		return false;
	}
	node->addr = addr;
	node->type = HINT_NODE_ADDR;
	node->addr_hints = records;
	r_rbtree_insert (user, &addr, &node->rb, hint_node_cmp, NULL);
	return true;
}