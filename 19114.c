R_API void r_core_anal_hint_print(RAnal* a, ut64 addr, int mode) {
	RBTree tree = NULL;
	ut64 hint_addr = UT64_MAX;
	const char *arch = r_anal_hint_arch_at(a, addr, &hint_addr);
	if (hint_addr != UT64_MAX) {
		print_arch_hint_cb (hint_addr, arch, &tree);
	}
	int bits = r_anal_hint_bits_at (a, addr, &hint_addr);
	if (hint_addr != UT64_MAX) {
		print_bits_hint_cb (hint_addr, bits, &tree);
	}
	const RVector *addr_hints = r_anal_addr_hints_at (a, addr);
	if (addr_hints) {
		print_addr_hint_cb (addr, addr_hints, &tree);
	}
	print_hint_tree (tree, mode);
	r_rbtree_free (tree, hint_node_free, NULL);
}