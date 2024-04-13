static void agraph_print_edge_gml(RANode *from, RANode *to, void *user) {
	r_cons_printf ("  edge [\n"
		"    source  %d\n"
		"    target  %d\n"
		"  ]\n", from->gnode->idx, to->gnode->idx
		);
}