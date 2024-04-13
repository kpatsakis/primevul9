bool print_bits_hint_cb(ut64 addr, int bits, void *user) {
	HintNode *node = R_NEW0 (HintNode);
	if (!node) {
		return false;
	}
	node->addr = addr;
	node->type = HINT_NODE_BITS;
	node->bits = bits;
	r_rbtree_insert (user, &addr, &node->rb, hint_node_cmp, NULL);
	return true;
}