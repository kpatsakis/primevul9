R_API void r_core_anal_hint_list(RAnal *a, int mode) {
	RBTree tree = NULL;
	// Collect all hints in the tree to sort them
	r_anal_arch_hints_foreach (a, print_arch_hint_cb, &tree);
	r_anal_bits_hints_foreach (a, print_bits_hint_cb, &tree);
	r_anal_addr_hints_foreach (a, print_addr_hint_cb, &tree);
	print_hint_tree (tree, mode);
	r_rbtree_free (tree, hint_node_free, NULL);
}