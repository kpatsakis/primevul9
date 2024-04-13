bool print_arch_hint_cb(ut64 addr, R_NULLABLE const char *arch, void *user) {
	HintNode *node = R_NEW0 (HintNode);
	if (!node) {
		return false;
	}
	node->addr = addr;
	node->type = HINT_NODE_ARCH;
	node->arch = arch;
	r_rbtree_insert (user, &addr, &node->rb, hint_node_cmp, NULL);
	return true;
}