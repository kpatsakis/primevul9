int hint_node_cmp(const void *incoming, const RBNode *in_tree, void *user) {
	ut64 ia = *(ut64 *)incoming;
	ut64 ta = container_of (in_tree, const HintNode, rb)->addr;
	if (ia < ta) {
		return -1;
	} else if (ia > ta) {
		return 1;
	}
	return 0;
}